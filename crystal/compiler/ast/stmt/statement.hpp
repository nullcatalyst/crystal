#pragma once

namespace crystal::compiler::ast::decl {

class VertexDeclaration;

}  // namespace crystal::compiler::ast::decl

namespace crystal::compiler::ast::stmt {

class Statement {
public:
  virtual ~Statement() = default;

  void indent_glsl(std::ostream& out, int indent) {
    for (int i = 0; i < indent; ++i) {
      out << "    ";
    }
  }

  virtual void to_glsl(std::ostream& out, decl::VertexDeclaration& vertex, int indent) = 0;
};

}  // namespace crystal::compiler::ast::stmt
