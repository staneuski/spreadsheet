#pragma once

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <iostream>
#include <functional>

class Sheet : public SheetInterface {
    using Row = std::vector<std::unique_ptr<Cell>>;

    class CellValuePrinter {
    public:
        CellValuePrinter(std::ostream& output) : out_(output) {}

        inline void operator()(const std::string& value) {
            out_ << value;
        }

        inline void operator()(double value) {
            out_ << value;
        }

        inline void operator()(FormulaError value) {
            out_ << value;
        }

    private:
        std::ostream& out_;
    };

public:
    ~Sheet() = default;

    void SetCell(Position pos, std::string text) override;

    inline const CellInterface* GetCell(Position pos) const override {
        return const_cast<Sheet&>(*this).GetCell(pos);
    }

    inline CellInterface* GetCell(Position pos) override {
        ThrowInvalidPosition(pos);
        return IsExist(pos) ? rows_[pos.row][pos.col].get() : nullptr;
    }

    void ClearCell(Position pos) override;

    inline Size GetPrintableSize() const override {
        return {
            static_cast<int>(rows_.size()),
            static_cast<int>(columns_count_)
        };
    }

    inline void PrintValues(std::ostream& output) const override {
        PrintCells(output, [&output](const std::unique_ptr<Cell>& cell) {
            if (cell)
                std::visit(CellValuePrinter(output), cell->GetValue());
        });
    }

    inline void PrintTexts(std::ostream& output) const override {
        PrintCells(output, [&output](const std::unique_ptr<Cell>& cell) {
            if (cell)
                output << cell->GetText();
        });
    }

private:
    std::vector<Row> rows_; // store cells by rows up to last not empty one
    size_t columns_count_ = {};

    inline static bool IsCellEmpty(const std::unique_ptr<Cell>& cell) {
        return !(cell && !cell->GetText().empty());
    }

    inline void ThrowInvalidPosition(const Position pos) const {
        if (!pos.IsValid())
            throw InvalidPositionException(
                "-> {" + std::to_string(pos.row)
                + ',' + std::to_string(pos.col)
                + '}'
            );
    }

    inline bool IsExist(const Position pos) const {
        return static_cast<size_t>(pos.row) < rows_.size()
            && static_cast<size_t>(pos.col) < rows_[pos.row].size();
    }

    // Корректирует размер таблицы, если позиция выходит за её размеры
    // и при необходимости увеличивает длину строки pos.row
    void Increase(Position pos);

    void Shrink();

    template<typename T, typename Fn>
    void DropTail(std::vector<T>& v, Fn is_empty);

    template<typename Predicate>
    void PrintCells(std::ostream& output, Predicate print_cell) const;
};

template<typename T, typename Predicate>
void Sheet::DropTail(std::vector<T>& v, Predicate predicate) {
    v.resize(std::distance(
        std::find_if_not(v.rbegin(), v.rend(), predicate),
        v.rend()
    ));
}

template <typename Predicate>
void Sheet::PrintCells(std::ostream& output, Predicate print_cell) const {
    for (const Row& row : rows_) {
        // Print row (rows stored to the last not empty cell) 
        if (!row.empty()) {
            print_cell(row.front());
            std::for_each(
                std::next(row.begin()),
                row.end(),
                [&](const std::unique_ptr<Cell>& cell) {
                    output << '\t';
                    print_cell(cell);
                }
            );
        }

        // Simulate dropped tail with tabs 
        for (size_t j = row.size(); j < columns_count_; ++j)
            output << '\t';

        output << '\n';
    }
}