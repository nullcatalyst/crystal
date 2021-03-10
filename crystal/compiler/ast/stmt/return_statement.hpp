#pragma once

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class ReturnStatement : public Statement {
  util::memory::Ref<expr::Expression> expr_;

public:
  ReturnStatement(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual output::PrintLambda to_glsl(decl::VertexDeclaration& vertex,
                                      uint32_t                 indent) const override;
  virtual output::PrintLambda to_glsl(decl::FragmentDeclaration& fragment,
                                      uint32_t                   indent) const override;
};

}  // namespace crystal::compiler::ast::stmt
