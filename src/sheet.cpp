#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <iostream>
#include <optional>

void Sheet::SetCell(Position pos, std::string text) {
    ThrowInvalidPosition(pos);
    Fit(pos);
    cells_.at(pos.row).at(pos.col)->Set(text);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return const_cast<Sheet&>(*this).GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    ThrowInvalidPosition(pos);
    if (!IsFit(pos))
        return nullptr;

    const std::unique_ptr<Cell>& cell_ptr = cells_.at(pos.row).at(pos.col);
    return IsCellEmpty(cell_ptr) ? cell_ptr.get() : nullptr;
}

void Sheet::ClearCell(Position pos) {
    ThrowInvalidPosition(pos);
    if (IsFit(pos) && cells_.at(pos.row).at(pos.col))
        cells_.at(pos.row).at(pos.col)->Clear();
}

Size Sheet::GetPrintableSize() const {
    Size printable_size;
    for (size_t i = 0; i < cells_.size(); ++i) {
        const std::vector<std::unique_ptr<Cell>>& row = cells_.at(i);
        if (!row.empty()) {
            if (!printable_size.rows)
                printable_size.rows = size_.rows - i;

            if (const int col_count = std::distance(
                    std::find_if_not(row.begin(), row.end(), IsCellEmpty),
                    row.end()
                );
                printable_size.cols < col_count
            )
                printable_size.cols = col_count;
        }
    }
    return printable_size;
}

void Sheet::PrintValues(std::ostream& output) const {
    PrintCells(output, [&output](const std::unique_ptr<Cell>& cell) {
        std::visit(CellValuePrinter(output), cell->GetValue());
    });
}

void Sheet::PrintTexts(std::ostream& output) const {
    PrintCells(output, [&output](const std::unique_ptr<Cell>& cell) {
        output << cell->GetText();
    });
}

void Sheet::Fit(const Position pos) {
    if (size_.rows - 1 < pos.row) {
        size_.rows = pos.row + 1;
        cells_.resize(size_.rows);
    }

    if (size_.cols - 1 < pos.col) {
        size_.cols = pos.col + 1;
        for (std::vector<std::unique_ptr<Cell>>& row : cells_)
            row.resize(size_.cols);
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}