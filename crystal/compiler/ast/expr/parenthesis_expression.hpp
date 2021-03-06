#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::expr {

class ParenthesisExpression : public Expression {
  util::memory::Ref<expr::Expression> expr_;

public:
  ParenthesisExpression(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual ~ParenthesisExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    return output::PrintLambda{
        [=](std::ostream& out) { out << "(" << expr_->to_glsl(opts) << ")"; }};
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    return output::PrintLambda{
        [=](std::ostream& out) { out << "(" << expr_->to_metal(opts) << ")"; }};
  }
};

}  // namespace crystal::compiler::ast::expr
