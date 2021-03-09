#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"

namespace crystal::compiler::ast::expr {

class IntegerExpression : public Expression {
  long long value_;

public:
  IntegerExpression(long long value) : value_(value) {}

  virtual ~IntegerExpression() = default;

  virtual void to_glsl(std::ostream& out) { out << value_; }
};

}  // namespace crystal::compiler::ast::expr
