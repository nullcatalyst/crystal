#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class ExpressionStatement : public Statement {
  util::memory::Ref<expr::Expression> expr_;

public:
  ExpressionStatement(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual output::PrintLambda to_glsl(const decl::VertexDeclaration& vertex) const override {
    return to_glsl();
  }

  virtual output::PrintLambda to_glsl(const decl::FragmentDeclaration& fragment) const override {
    return to_glsl();
  }

  virtual output::PrintLambda to_pretty_glsl(const decl::VertexDeclaration& vertex,
                                             uint32_t                       indent) const override {
    return to_pretty_glsl(indent);
  }

  virtual output::PrintLambda to_pretty_glsl(const decl::FragmentDeclaration& fragment,
                                             uint32_t indent) const override {
    return to_pretty_glsl(indent);
  }

private:
  output::PrintLambda to_glsl() const {
    return output::PrintLambda{[=](std::ostream& out) { out << expr_->to_glsl() << ";"; }};
  }

  output::PrintLambda to_pretty_glsl(uint32_t indent) const {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl_indent{indent} << expr_->to_glsl() << ";\n";
    }};
  }
};

}  // namespace crystal::compiler::ast::stmt
