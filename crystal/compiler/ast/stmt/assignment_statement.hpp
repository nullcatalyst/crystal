#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"

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

  virtual output::PrintLambda to_glsl(decl::VertexDeclaration& vertex,
                                      uint32_t                 indent) const override {
    return to_glsl(indent);
  }

  virtual output::PrintLambda to_glsl(decl::FragmentDeclaration& fragment,
                                      uint32_t                   indent) const override {
    return to_glsl(indent);
  }

private:
  output::PrintLambda to_glsl(uint32_t indent) const;
};

}  // namespace crystal::compiler::ast::stmt
