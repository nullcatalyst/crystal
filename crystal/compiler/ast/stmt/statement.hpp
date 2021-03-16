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

  virtual output::PrintLambda to_glsl(const decl::VertexDeclaration& vertex) const = 0;

  virtual output::PrintLambda to_glsl(const decl::FragmentDeclaration& fragment) const = 0;

  virtual output::PrintLambda to_pretty_glsl(const decl::VertexDeclaration& vertex,
                                             uint32_t                       indent) const = 0;

  virtual output::PrintLambda to_pretty_glsl(const decl::FragmentDeclaration& fragment,
                                             uint32_t                         indent) const = 0;

  virtual output::PrintLambda to_metal(const decl::VertexDeclaration& vertex,
                                       uint32_t                       indent) const = 0;

  virtual output::PrintLambda to_metal(const decl::FragmentDeclaration& fragment,
                                       uint32_t                         indent) const = 0;
};

}  // namespace crystal::compiler::ast::stmt
