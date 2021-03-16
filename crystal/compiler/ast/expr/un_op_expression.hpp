#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"
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

  virtual output::PrintLambda to_glsl() const override {
    switch (op_) {
      case UnOp::Pos:
        return output::PrintLambda{
            [rhs = rhs_](std::ostream& out) { out << "+" << rhs->to_glsl(); }};

      case UnOp::Neg:
        return output::PrintLambda{
            [rhs = rhs_](std::ostream& out) { out << "-" << rhs->to_glsl(); }};

      default:
        util::msg::fatal("unhandled unary operator [", static_cast<uint32_t>(op_), "]");
        break;
    }
  }

  virtual output::PrintLambda to_metal() const override {
    switch (op_) {
      case UnOp::Pos:
        return output::PrintLambda{
            [rhs = rhs_](std::ostream& out) { out << "+" << rhs->to_metal(); }};

      case UnOp::Neg:
        return output::PrintLambda{
            [rhs = rhs_](std::ostream& out) { out << "-" << rhs->to_metal(); }};

      default:
        util::msg::fatal("unhandled unary operator [", static_cast<uint32_t>(op_), "]");
        break;
    }
  }
};

}  // namespace crystal::compiler::ast::expr
