#include "cell.h"

#include <cassert>

void Cell::Set(std::string text) {
    std::unique_ptr<Impl> updated_impl = std::make_unique<EmptyImpl>();
    if (!text.empty() && text[0] == FORMULA_SIGN && text.size() > 1u)
        updated_impl = std::make_unique<FormulaImpl>(sheet_, text.substr(1));
    else if (!text.empty())
        updated_impl = std::make_unique<TextImpl>(text);

    Validate(updated_impl);

    DropDependentCache();
    UpdateCellsGraph(updated_impl);

    impl_ = std::move(updated_impl);
}

void Cell::Validate(const std::vector<Position>& referenced_cells,
                    std::unordered_set<const Cell*>& validated_cells) const {
    for (const Position& pos : referenced_cells) {
        const Cell* ref_cell = reinterpret_cast<Cell*>(sheet_.GetCell(pos));

        if (this == ref_cell) {
            throw CircularDependencyException("circular dependency");
        } else if (
            ref_cell && validated_cells.find(ref_cell) != validated_cells.end()
        ) {
            validated_cells.insert(ref_cell);
            Validate(ref_cell->GetReferencedCells(), validated_cells);
        }
    }
}

void Cell::DropDependentCache(
    std::unordered_set<const Cell*>& dropped_cache_cells
) {
    for (Cell* dependent_cell : dependent_cells_) {
        if (dropped_cache_cells.find(dependent_cell) != dropped_cache_cells.end()) {
            dependent_cell->impl_->DropCache();
            dropped_cache_cells.insert(dependent_cell);

            dependent_cell->DropDependentCache(dropped_cache_cells);
        }
    }
}

void Cell::UpdateCellsGraph(const std::unique_ptr<Impl>& updated_impl) {
    // Erase this cell in all currently dependent cells
    for (const Position& referenced_pos : referenced_cells_)
        if (Cell* referenced_cell = reinterpret_cast<Cell*>(
            sheet_.GetCell(referenced_pos)
        ))
            referenced_cell->dependent_cells_.erase(this);

    // Update referenced cells and insert this cell as dependent one
    referenced_cells_ = updated_impl->GetReferencedCells();
    for (const Position& referenced_pos : referenced_cells_)
        if (Cell* referenced_cell = reinterpret_cast<Cell*>(
            sheet_.GetCell(referenced_pos)
        ))
            referenced_cell->dependent_cells_.insert(this);
}