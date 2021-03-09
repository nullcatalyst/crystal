#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::expr {

enum class UnOp {
  Undefined = 0,
  Pos,
  Neg,
};

class UnOpExpression : public Expression {
  util::memory::Ref<expr::Expression> rhs_;
  UnOp                                op_;

public:
  UnOpExpression(util::memory::Ref<expr::Expression> rhs, UnOp op) : rhs_(rhs), op_(op) {}

  virtual ~UnOpExpression() = default;

  virtual void to_glsl(std::ostream& out) {
    switch (op_) {
      case UnOp::Pos:
        out << "+";
        rhs_->to_glsl(out);
        break;

      case UnOp::Neg:
        out << "-";
        rhs_->to_glsl(out);
        break;

      default:
        break;
    }
  }
};

}  // namespace crystal::compiler::ast::expr
