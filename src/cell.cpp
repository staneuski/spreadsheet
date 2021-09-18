#include "cell.h"

#include <cassert>

void Cell::Set(std::string text) {
    if (text.empty()) {
        Clear();
        return;
    }

    if (text.front() == FORMULA_SIGN && text.size() > 1u)
        impl_ = std::make_unique<FormulaImpl>(sheet_, text.substr(1));
    else
        impl_ = std::make_unique<TextImpl>(text);
}
