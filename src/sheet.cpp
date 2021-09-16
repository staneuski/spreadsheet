#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <iostream>
#include <optional>

void Sheet::SetCell(Position pos, std::string text) {
    ThrowInvalidPosition(pos);

    Fit(pos);

    std::unique_ptr<Cell>& cell = rows_[pos.row][pos.col];
    if (!cell)
        cell = std::make_unique<Cell>();
    cell->Set(text);
}

CellInterface* Sheet::GetCell(Position pos) {
    ThrowInvalidPosition(pos);

    const std::unique_ptr<Cell>& cell = rows_[pos.row][pos.col];
    return IsFit(pos) && !IsCellEmpty(cell) ? cell.get() : nullptr;
}

void Sheet::ClearCell(Position pos) {
    ThrowInvalidPosition(pos);
    if (IsFit(pos) && rows_[pos.row][pos.col]) {
        Row& row = rows_[pos.row];
        row[pos.col]->Clear();
        if (row.size() == static_cast<size_t>(pos.col + 1))
            Shrink();
    }
}

Size Sheet::GetPrintableSize() const {
    Size printable_size;

    auto row = rows_.rbegin();
    for (; row != rows_.rend(); row = std::next(row)) {
        if (row->empty())
            break;

        const size_t index = std::distance(
            row->begin(),
            std::find_if_not(row->begin(), row->end(), IsCellEmpty)
        );
        printable_size.cols = std::max(
            static_cast<int>(columns_count_ - index),
            printable_size.cols
        );
    }

    printable_size.rows = std::distance(rows_.rbegin(), row);
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

void Sheet::Fit(Position pos) {
    rows_.resize(std::max(rows_.size(), static_cast<size_t>(pos.row + 1)));

    Row& row = rows_.at(pos.row);
    row.resize(std::max(row.size(), static_cast<size_t>(pos.col + 1)));

    columns_count_ = std::max(row.size(), columns_count_);
}

void Sheet::Shrink() {
    columns_count_ = {};
    for (Row& row : rows_) {
        DropTail(row, IsCellEmpty);
        columns_count_ = std::max(row.size(), columns_count_);
    }
    DropTail(rows_, [](const auto& row) { return row.empty(); });
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}