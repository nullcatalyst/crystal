#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::expr {

class PropertyExpression : public Expression {
  util::memory::Ref<expr::Expression> expr_;
  std::string                         name_;

public:
  PropertyExpression(util::memory::Ref<expr::Expression> expr, std::string_view name)
      : expr_(expr), name_(name) {}

  virtual ~PropertyExpression() = default;

  virtual void to_glsl(std::ostream& out) {
    expr_->to_glsl(out);
    out << "." << name_;
  }
};

}  // namespace crystal::compiler::ast::expr
