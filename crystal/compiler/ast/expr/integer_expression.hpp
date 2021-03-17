#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class IntegerExpression : public Expression {
  long long value_;

public:
  IntegerExpression(long long value) : value_(value) {}

  virtual ~IntegerExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) { out << value_; }};
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) { out << value_; }};
  }
};

}  // namespace crystal::compiler::ast::expr
