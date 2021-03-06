#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::expr {

class PropertyExpression : public Expression {
  util::memory::Ref<expr::Expression> expr_;
  std::string                         name_;

public:
  PropertyExpression(util::memory::Ref<expr::Expression> expr, std::string_view name)
      : expr_(expr), name_(name) {}

  virtual ~PropertyExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    return output::PrintLambda{
        [=](std::ostream& out) { out << expr_->to_glsl(opts) << "." << name_; }};
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    return output::PrintLambda{
        [=](std::ostream& out) { out << expr_->to_metal(opts) << "." << name_; }};
  }
};

}  // namespace crystal::compiler::ast::expr
