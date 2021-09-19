#include "cell.h"

#include <cassert>

void Cell::Set(std::string text) {
    if (text.empty()) {
        Clear();
        return;
    }

    std::unique_ptr<Impl> updated_impl = std::make_unique<EmptyImpl>();
    if (!text.empty() && text[0] == FORMULA_SIGN && text.size() > 1u)
        updated_impl = std::make_unique<FormulaImpl>(sheet_, text.substr(1));
    else if (!text.empty())
        updated_impl = std::make_unique<TextImpl>(text);

    UpdateCellsGraph(updated_impl);

    impl_ = std::move(updated_impl);
}

void Cell::UpdateCellsGraph(const std::unique_ptr<Impl>& updated_impl) {
    referenced_cells_ = updated_impl->GetReferencedCells();
}