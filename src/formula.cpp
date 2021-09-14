#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression)
        : ast_(ParseFormulaAST(expression)) {
    }

    Value Evaluate() const override {
        try {
            return ast_.Execute();
        } catch (const FormulaError& formula_error) {
            return formula_error;
        }
    }

    std::string GetExpression() const override {
        std::stringstream ss;
        ast_.PrintFormula(ss);
        return ss.str();
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}