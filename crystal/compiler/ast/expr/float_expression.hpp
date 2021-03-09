#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"

namespace crystal::compiler::ast::expr {

class FloatExpression : public Expression {
  long double value_;

public:
  FloatExpression(long double value) : value_(value) {}

  virtual ~FloatExpression() = default;

  virtual void to_glsl(std::ostream& out) { out << value_; }
};

}  // namespace crystal::compiler::ast::expr
