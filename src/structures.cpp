#include "common.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

std::string Position::ToString() const {
    if (!IsValid())
        return {};

    size_t column_index = col + 1;

    std::string column_name = {};
    while (column_index > 0) {
        const int modulo = (column_index - 1) % LETTERS;
        column_name = static_cast<char>('A' + modulo) + column_name;
        column_index = (column_index - modulo)/LETTERS;
    }
    return column_name + std::to_string(row + 1);
}

Position Position::FromString(std::string_view str) {
    if (str.size() > MAX_POSITION_LENGTH)
        return Position::NONE;

    const auto pos = std::find_if(str.begin(), str.end(), [](const char c) {
        return !(std::isalpha(c) && std::isupper(c));
    });
    std::string_view column_name = str.substr(0, pos - str.begin());
    std::string_view row_name = str.substr(pos - str.begin());

    const auto& convert_column = [](std::string_view column_name) {
        int column_index = 0;
        for (
            auto it = column_name.begin();
            it != column_name.end();
            it = std::next(it)
        )
            column_index += (*it - 'A' + 1)*std::pow(
                LETTERS,
                std::distance(it, column_name.end()) - 1
            );

        return column_index - 1;
    };

    const auto& convert_row = [](std::string_view row_name) {
        return std::stoi(std::string(row_name)) - 1;
    };

    if (column_name.empty() || row_name.empty())
        return Position::NONE;
    else if (
        column_name.size() > MAX_POS_LETTER_COUNT
        || std::any_of(row_name.begin(), row_name.end(), [](const char c) {
            return !std::isdigit(c);
        })
    )
        return Position::NONE;
    else
        return {convert_row(row_name), convert_column(column_name)};
}