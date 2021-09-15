#pragma once

#include "common.h"
#include "formula.h"

#include <iostream>
#include <optional>
#include <string>

class Cell final : public CellInterface {
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
    };

    class EmptyImpl final : public Impl {
    public:
        inline CellInterface::Value GetValue() const override {
            return {};
        }

        inline std::string GetText() const override {
            return {};
        }
    };

    class TextImpl final : public Impl  {
    public:
        TextImpl(std::string text) : content_(std::move(text)) {}

        inline CellInterface::Value GetValue() const override {
            return content_.front() == ESCAPE_SIGN
                   ? content_.substr(1)
                   : content_;
        }

        inline std::string GetText() const override {
            return content_;
        }

    private:
        std::string content_;
    };

    class FormulaImpl final : public Impl {
    public:
        FormulaImpl(std::string text) : formula_(ParseFormula(text)) {}

        inline CellInterface::Value GetValue() const override {
            const FormulaInterface::Value result = formula_->Evaluate();
            return std::holds_alternative<double>(result)
                   ? CellInterface::Value(std::get<double>(result))
                   : CellInterface::Value(std::get<FormulaError>(result));
        }

        inline std::string GetText() const override {
            return FORMULA_SIGN + formula_->GetExpression();
        }

    private:
        std::unique_ptr<FormulaInterface> formula_;
    };

public:
    Cell() = default;
    ~Cell() = default;

    inline Value GetValue() const override {
        return impl_->GetValue();
    }

    inline std::string GetText() const override {
        return impl_->GetText();
    }

    inline void Clear() {
        impl_ = std::make_unique<EmptyImpl>();
    }

    void Set(std::string text) override;

private:
    std::unique_ptr<Impl> impl_ = std::make_unique<EmptyImpl>();
};
