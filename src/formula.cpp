#include "formula.h"

#include "FormulaAST.h"

#include <cassert>
#include <cctype>
#include <sstream>

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
class Formula : public FormulaInterface {
    class ValueGetter {
    public:
        ValueGetter(double& value) : value_(value) {}

        inline void operator()(const double value) {
            value_ = value;
        }

        inline void operator()(const std::string& text) {
            try {
                value_ = std::stod(text);
            } catch (const std::invalid_argument&) {
                throw FormulaError(FormulaError::Category::Value);
            }
        }

        inline void operator()(const FormulaError& error) {
            throw error;
        }

    private:
        double& value_;
    };

public:
    explicit Formula(std::string expression)
        : ast_(ParseFormulaAST(expression)) {
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        const FormulaAST::ValueGetter& get_value = [&sheet](Position pos) {
            if (!pos.IsValid())
                throw FormulaError(FormulaError::Category::Ref);

            const CellInterface* cell = sheet.GetCell(pos);
            if (!cell)
                return .0;

            double value = {};
            const auto& cell_value = cell->GetValue();
            std::visit(ValueGetter(value), cell_value);
            return value;
        };

        try {
            return ast_.Execute(get_value);
        } catch (const FormulaError& formula_error) {
            return formula_error;
        }
    }

    std::string GetExpression() const override {
        std::stringstream ss;
        ast_.PrintFormula(ss);
        return ss.str();
    }

    inline std::vector<Position> GetReferencedCells() const override {
        const std::forward_list<Position>& referenced_cells = ast_.GetCells();
        return {referenced_cells.begin(), referenced_cells.end()};
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}