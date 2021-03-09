#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast::stmt {

enum class AssignmentOp {
  Undefined = 0,
  Set,
  Add,
  Sub,
  Mul,
  Div,
};

class AssignmentStatement : public Statement {
  util::memory::Ref<expr::Expression> var_;
  util::memory::Ref<expr::Expression> value_;
  AssignmentOp                        op_;

public:
  AssignmentStatement(util::memory::Ref<expr::Expression> var,
                      util::memory::Ref<expr::Expression> value, AssignmentOp op)
      : var_(var), value_(value), op_(op) {}

  virtual void to_glsl(std::ostream& out, decl::VertexDeclaration& vertex, int indent) override {
    indent_glsl(out, indent);
    var_->to_glsl(out);

    switch (op_) {
      case AssignmentOp::Set:
        out << " = ";
        break;
      case AssignmentOp::Add:
        out << " += ";
        break;
      case AssignmentOp::Sub:
        out << " -= ";
        break;
      case AssignmentOp::Mul:
        out << " *= ";
        break;
      case AssignmentOp::Div:
        out << " /= ";
        break;
      default:
        util::msg::fatal("unhandled assignment operator [", static_cast<uint32_t>(op_), "]");
    }

    value_->to_glsl(out);
    out << ";\n";
  }
};

}  // namespace crystal::compiler::ast::stmt
