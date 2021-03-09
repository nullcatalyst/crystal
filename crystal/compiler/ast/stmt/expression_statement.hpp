#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class ExpressionStatement : public Statement {
  util::memory::Ref<expr::Expression> expr_;

public:
  ExpressionStatement(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual void to_glsl(std::ostream& out, decl::VertexDeclaration& vertex, int indent) override {
    indent_glsl(out, indent);
    expr_->to_glsl(out);
    out << ";\n";
  }
};

}  // namespace crystal::compiler::ast::stmt
