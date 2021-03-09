#pragma once

#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

class ReturnStatement : public Statement {
  util::memory::Ref<expr::Expression> expr_;

public:
  ReturnStatement(util::memory::Ref<expr::Expression> expr) : expr_(expr) {}

  virtual void to_glsl(std::ostream& out, decl::VertexDeclaration& vertex, int indent) override {
    indent_glsl(out, indent);
    out << "const " << vertex.return_type()->name() << " _ = ";
    expr_->to_glsl(out);
    out << ";\n";

    const util::memory::Ref<type::StructType> return_struct_type = vertex.return_type();
    for (auto& prop : return_struct_type->properties()) {
      indent_glsl(out, indent);

      if (prop.index < 0) {
        // Skip properties that don't have an output index.
        continue;
      }

      if (prop.index == 0) {
        // The zero output for the vertex function must be the vertex position.
        out << "gl_Position = _." << prop.name << ";\n";
        continue;
      }

      out << "o_" << prop.name << " = _." << prop.name << ";\n";
    }

    indent_glsl(out, indent);
    out << "return;\n";
  }
};

}  // namespace crystal::compiler::ast::stmt
