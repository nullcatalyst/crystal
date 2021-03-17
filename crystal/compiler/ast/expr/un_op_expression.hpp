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

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    switch (op_) {
      case UnOp::Pos:
        return output::PrintLambda{[=](std::ostream& out) { out << "+" << rhs_->to_glsl(opts); }};

      case UnOp::Neg:
        return output::PrintLambda{[=](std::ostream& out) { out << "-" << rhs_->to_glsl(opts); }};

      default:
        util::msg::fatal("unhandled unary operator [", static_cast<uint32_t>(op_), "]");
        break;
    }
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    switch (op_) {
      case UnOp::Pos:
        return output::PrintLambda{[=](std::ostream& out) { out << "+" << rhs_->to_metal(opts); }};

      case UnOp::Neg:
        return output::PrintLambda{[=](std::ostream& out) { out << "-" << rhs_->to_metal(opts); }};

      default:
        util::msg::fatal("unhandled unary operator [", static_cast<uint32_t>(op_), "]");
        break;
    }
  }
};

}  // namespace crystal::compiler::ast::expr
