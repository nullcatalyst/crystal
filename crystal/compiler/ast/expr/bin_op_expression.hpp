#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/memory/ref_count.hpp"
#include "util/msg/msg.hpp"

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

  virtual output::PrintLambda to_glsl() const override {
    switch (op_) {
      case BinOp::Add:
        return output::PrintLambda{
            [=](std::ostream& out) { out << lhs_->to_glsl() << "+" << rhs_->to_glsl(); }};

      case BinOp::Sub:
        return output::PrintLambda{
            [=](std::ostream& out) { out << lhs_->to_glsl() << "-" << rhs_->to_glsl(); }};

      case BinOp::Mul:
        return output::PrintLambda{
            [=](std::ostream& out) { out << lhs_->to_glsl() << "*" << rhs_->to_glsl(); }};

      case BinOp::Div:
        return output::PrintLambda{
            [=](std::ostream& out) { out << lhs_->to_glsl() << "/" << rhs_->to_glsl(); }};

      default:
        util::msg::fatal("unhandled binary operator [", static_cast<uint32_t>(op_), "]");
        break;
    }
  }
};

}  // namespace crystal::compiler::ast::expr
