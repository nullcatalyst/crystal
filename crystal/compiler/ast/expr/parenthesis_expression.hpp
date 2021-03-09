#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::expr {

class ParenthesisExpression : public Expression {
  util::memory::Ref<expr::Expression> expr_;

public:
  ParenthesisExpression(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual ~ParenthesisExpression() = default;

  virtual void to_glsl(std::ostream& out) {
    out << "(";
    expr_->to_glsl(out);
    out << ")";
  }
};

}  // namespace crystal::compiler::ast::expr
