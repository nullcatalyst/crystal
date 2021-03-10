#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class FloatExpression : public Expression {
  long double value_;

public:
  FloatExpression(long double value) : value_(value) {}

  virtual ~FloatExpression() = default;

  virtual output::PrintLambda to_glsl() const override {
    return output::PrintLambda{[value = value_](std::ostream& out) { out << value; }};
  }
};

}  // namespace crystal::compiler::ast::expr
