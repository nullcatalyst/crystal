#pragma once

#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::decl {

class VertexDeclaration;
class FragmentDeclaration;

}  // namespace crystal::compiler::ast::decl

namespace crystal::compiler::ast::stmt {

class Statement {
public:
  virtual ~Statement() = default;

  virtual output::PrintLambda to_glsl(decl::VertexDeclaration& vertex, uint32_t indent) const {
    return output::PrintLambda{};
  }

  virtual output::PrintLambda to_glsl(decl::FragmentDeclaration& fragment, uint32_t indent) const {
    return output::PrintLambda{};
  }
};

}  // namespace crystal::compiler::ast::stmt
