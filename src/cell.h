#pragma once

#include "common.h"
#include "formula.h"

#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>

class Cell final : public CellInterface {
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual CellInterface::Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual void DropCache() = 0;
        virtual std::vector<Position> GetReferencedCells() const = 0;
    };

    class EmptyImpl final : public Impl {
    public:
        inline CellInterface::Value GetValue() const override {
            return {};
        }

        inline std::string GetText() const override {
            return {};
        }

        inline void DropCache() override {}

        inline std::vector<Position> GetReferencedCells() const override {
            return {};
        }
    };

    class TextImpl final : public Impl {
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

        inline void DropCache() override {}

        inline std::vector<Position> GetReferencedCells() const override {
            return {};
        }

    private:
        std::string content_;
    };

    class FormulaImpl final : public Impl {
    public:
        FormulaImpl(const SheetInterface& sheet, std::string text)
            : sheet_(sheet)
            , formula_(ParseFormula(text)) {
        }

        inline CellInterface::Value GetValue() const override {
            const FormulaInterface::Value& result = (cache_ != std::nullopt)
                ? *cache_ : formula_->Evaluate(sheet_);
            return std::holds_alternative<double>(result)
                   ? CellInterface::Value(std::get<double>(result))
                   : CellInterface::Value(std::get<FormulaError>(result));
        }

        inline std::string GetText() const override {
            return FORMULA_SIGN + formula_->GetExpression();
        }

        inline void DropCache() override {
            cache_ = std::nullopt;
        }

        inline std::vector<Position> GetReferencedCells() const override {
            return formula_->GetReferencedCells();
        }

    private:
        const SheetInterface& sheet_;
        std::unique_ptr<FormulaInterface> formula_;
        std::optional<FormulaInterface::Value> cache_;
    };

public:
    Cell(SheetInterface& sheet) : sheet_(sheet) {};

    ~Cell() = default;

    inline Value GetValue() const override {
        return impl_->GetValue();
    }

    inline std::string GetText() const override {
        return impl_->GetText();
    }

    inline std::vector<Position> GetReferencedCells() const override {
        return referenced_cells_;
    }

    inline void Clear() {
        impl_ = std::make_unique<EmptyImpl>();
    }

    void Set(std::string text) override;

private:
    SheetInterface& sheet_;
    std::unique_ptr<Impl> impl_ = std::make_unique<EmptyImpl>();
    std::vector<Position> referenced_cells_;
    std::unordered_set<Cell*> dependent_cells_;

    inline void Validate(const std::unique_ptr<Impl>& updated_impl) const {
        std::unordered_set<const Cell*> validated_cells = {};
        Validate(updated_impl->GetReferencedCells(), validated_cells);
    }

    inline void DropDependentCache() {
        std::unordered_set<const Cell*> dropped_cache_cells = {};
        DropDependentCache(dropped_cache_cells);
    }

    void Validate(const std::vector<Position>& referenced_cells,
                  std::unordered_set<const Cell*>& validated_cells) const;

    void DropDependentCache(
        std::unordered_set<const Cell*>& dropped_cache_cells
    );

    void UpdateCellsGraph(const std::unique_ptr<Impl>& updated_impl);
};
