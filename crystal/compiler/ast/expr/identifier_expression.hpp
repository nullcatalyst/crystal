#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"

namespace crystal::compiler::ast::expr {

class IdentifierExpression : public Expression {
  std::string name_;

public:
  IdentifierExpression(std::string_view name) : name_(name) {}

  virtual ~IdentifierExpression() = default;

  virtual void to_glsl(std::ostream& out) { out << "_" << name_; }
};

}  // namespace crystal::compiler::ast::expr
