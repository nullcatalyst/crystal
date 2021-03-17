#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class IdentifierExpression : public Expression {
  std::string name_;

public:
  IdentifierExpression(std::string_view name) : name_(name) {}

  virtual ~IdentifierExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) { out << output::glsl::mangle_name{name_}; }};
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    return output::PrintLambda{
        [=](std::ostream& out) { out << output::metal::mangle_name{name_}; }};
  }
};

}  // namespace crystal::compiler::ast::expr
