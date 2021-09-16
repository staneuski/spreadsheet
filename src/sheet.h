#pragma once

#include "cell.h"
#include "common.h"

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

    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    std::vector<Row> rows_;
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

    inline bool IsFit(const Position pos) const {
        return rows_.size() >= static_cast<size_t>(pos.row + 1)
            && columns_count_ >= static_cast<size_t>(pos.col + 1)
            && rows_.at(pos.row).size() >= static_cast<size_t>(pos.col + 1);
    }

    // Корректирует размер таблицы, если позиция выходит за её размеры
    // и при необходимости увеличивает длину строки pos.row
    void Fit(Position pos);

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
    const Size printable_size = GetPrintableSize();
    for (int i = 0; i < printable_size.rows; ++i) {
        bool is_first = true;
        for (int j = 0; j < printable_size.cols; ++j) {
            if (!is_first)
                output << '\t';
            else
                is_first = false;

            if (const std::unique_ptr<Cell>& cell = rows_.at(i).at(j))
                print_cell(cell);
        }
        output << '\n';
    }
}