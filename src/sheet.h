#pragma once

#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
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

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    std::vector<std::vector<std::unique_ptr<Cell>>> cells_; // rows x cols
    Size size_;

    inline void ThrowInvalidPosition(const Position pos) const {
        if (!pos.IsValid())
            throw InvalidPositionException(
                "-> {" + std::to_string(pos.row)
                + ',' + std::to_string(pos.col)
                + '}'
            );
    }

    inline static bool IsCellEmpty(const std::unique_ptr<Cell>& cell) {
        return cell && !cell->GetText().empty();
    }

    inline bool IsFit(const Position pos) const {
        return size_.rows - 1 >= pos.row && size_.cols - 1 >= pos.col;
    }

    // Корректирует размер таблицы, если позиция выходит за её размеры
    void Fit(const Position pos);

    template <typename CellPredicate>
    void PrintCells(std::ostream& output, CellPredicate fn) const;
};

template <typename CellPredicate>
void Sheet::PrintCells(std::ostream& output, CellPredicate fn) const {
    const Size printable_size = GetPrintableSize();
    for (int i = 0; i < printable_size.rows; ++i) {
        bool is_first = true;
        for (int j = 0; j < printable_size.cols; ++j) {
            if (!is_first)
                output << '\t';
            else
                is_first = false;

            if (const std::unique_ptr<Cell>& cell = cells_.at(i).at(j))
                fn(cell);
        }
        output << '\n';
    }
}