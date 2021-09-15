/* common.h
Объявления вспомогательных структур.
 */

#pragma once

#include <iosfwd>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <string>
#include <variant>

// Описывает ошибки, которые могут возникнуть при вычислении формулы.
class FormulaError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

std::ostream& operator<<(std::ostream& output, FormulaError fe);

// Исключение, выбрасываемое при попытке задать синтаксически некорректную
// формулу
class FormulaException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

inline constexpr char FORMULA_SIGN = '=';
inline constexpr char ESCAPE_SIGN = '\'';

class CellInterface {
public:
    // Либо текст ячейки, либо значение формулы, либо сообщение об ошибке из
    // формулы
    using Value = std::variant<std::string, double, FormulaError>;

    virtual ~CellInterface() = default;

    // Задаёт содержимое ячейки. Если текст начинается со знака "=", то он
    // интерпретируется как формула. Уточнения по записи формулы:
    // * Если текст содержит только символ "=" и больше ничего, то он не считается
    // формулой
    // * Если текст начинается с символа "'" (апостроф), то при выводе значения
    // ячейки методом GetValue() он опускается. Можно использовать, если нужно
    // начать текст со знака "=", но чтобы он не интерпретировался как формула.
    virtual void Set(std::string text) = 0;

    // Возвращает видимое значение ячейки.
    // В случае текстовой ячейки это её текст (без экранирующих символов). В
    // случае формулы - числовое значение формулы или сообщение об ошибке.
    virtual Value GetValue() const = 0;
    // Возвращает внутренний текст ячейки, как если бы мы начали её
    // редактирование. В случае текстовой ячейки это её текст (возможно,
    // содержащий экранирующие символы). В случае формулы - её выражение.
    virtual std::string GetText() const = 0;
};

// Позиция ячейки. Индексация с нуля.
struct Position {
    int row = 0;
    int col = 0;

    inline bool operator==(Position rhs) const {
        return row == rhs.row && col == rhs.col;
    }

    inline bool operator<(Position rhs) const {
        return row < rhs.row && col < rhs.col;
    }

    inline bool IsValid() const {
        return row >= 0 && col >= 0 && row < MAX_ROWS && col < MAX_COLS;
    }

    std::string ToString() const;

    static Position FromString(std::string_view str);

    static const int MAX_ROWS = 16384;
    static const int MAX_COLS = 16384;
    static const Position NONE;
};