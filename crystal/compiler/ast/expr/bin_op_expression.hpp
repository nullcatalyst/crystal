#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::expr {

enum class BinOp {
  Undefined = 0,
  Add,
  Sub,
  Mul,
  Div,
};

class BinOpExpression : public Expression {
  util::memory::Ref<expr::Expression> lhs_;
  util::memory::Ref<expr::Expression> rhs_;
  BinOp                               op_;

public:
  BinOpExpression(util::memory::Ref<expr::Expression> lhs, util::memory::Ref<expr::Expression> rhs,
                  BinOp op)
      : lhs_(lhs), rhs_(rhs), op_(op) {}

  virtual ~BinOpExpression() = default;

  virtual void to_glsl(std::ostream& out) {
    switch (op_) {
      case BinOp::Add:
        lhs_->to_glsl(out);
        out << "+";
        rhs_->to_glsl(out);
        break;

      case BinOp::Sub:
        lhs_->to_glsl(out);
        out << "-";
        rhs_->to_glsl(out);
        break;

      case BinOp::Mul:
        lhs_->to_glsl(out);
        out << "*";
        rhs_->to_glsl(out);
        break;

      case BinOp::Div:
        lhs_->to_glsl(out);
        out << "/";
        rhs_->to_glsl(out);
        break;

      default:
        break;
    }
  }
};

}  // namespace crystal::compiler::ast::expr
