#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class IdentifierExpression : public Expression {
  std::string name_;

public:
  IdentifierExpression(std::string_view name) : name_(name) {}

  virtual ~IdentifierExpression() = default;

  virtual output::PrintLambda to_glsl() const override {
    return output::PrintLambda{
        [name = name_](std::ostream& out) { out << output::glsl_mangle_name{name}; }};
  }
};

}  // namespace crystal::compiler::ast::expr
