#include "sheet.h"

void Sheet::SetCell(Position pos, std::string text) {
    ThrowInvalidPosition(pos);

    Increase(pos);

    std::unique_ptr<Cell>& cell = rows_[pos.row][pos.col];
    if (!cell)
        cell = std::make_unique<Cell>(*this);
    cell->Set(text);
}

void Sheet::ClearCell(Position pos) {
    ThrowInvalidPosition(pos);
    if (IsExist(pos) && rows_[pos.row][pos.col]) {
        Row& row = rows_[pos.row];
        row[pos.col]->Clear();
        if (row.size() == static_cast<size_t>(pos.col + 1))
            Shrink();
    }
}

void Sheet::Increase(Position pos) {
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