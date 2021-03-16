#include "crystal/compiler/ast/stmt/return_statement.hpp"

#include "crystal/compiler/ast/decl/fragment_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "crystal/compiler/ast/stmt/statement.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast::stmt {

output::PrintLambda ReturnStatement::to_glsl(const decl::VertexDeclaration& vertex) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << vertex.return_type()->name() << " _=" << expr_->to_glsl() << ";";

    const util::memory::Ref<type::StructType> return_struct_type = vertex.return_type();
    for (auto& prop : return_struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an output index.
        continue;
      }

      out << output::glsl::varying_name{static_cast<uint32_t>(prop.index), prop.name} << "=_."
          << prop.name << ";";

      if (prop.index == 0) {
        // The zero output for the vertex function must be the vertex position.
        out << "gl_Position=_." << prop.name << ";";
      }
    }

    out << "return;";
  }};
}

output::PrintLambda ReturnStatement::to_glsl(const decl::FragmentDeclaration& fragment) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << fragment.return_type()->name() << " _=" << expr_->to_glsl() << ";";

    const util::memory::Ref<type::StructType> return_struct_type = fragment.return_type();
    for (auto& prop : return_struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an output index.
        continue;
      }

      out << output::glsl::fragment_output_name{static_cast<uint32_t>(prop.index), prop.name}
          << "=_." << prop.name << ";";
    }

    out << "return;";
  }};
}

output::PrintLambda ReturnStatement::to_pretty_glsl(const decl::VertexDeclaration& vertex,
                                                    uint32_t                       indent) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << output::glsl::indent{indent} << vertex.return_type()->name()
        << " _ = " << expr_->to_glsl() << ";\n";

    const util::memory::Ref<type::StructType> return_struct_type = vertex.return_type();
    for (auto& prop : return_struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an output index.
        continue;
      }

      out << output::glsl::indent{indent}
          << output::glsl::varying_name{static_cast<uint32_t>(prop.index), prop.name} << " = _."
          << prop.name << ";\n";

      if (prop.index == 0) {
        // The zero output for the vertex function must be the vertex position.
        out << output::glsl::indent{indent} << "gl_Position = _." << prop.name << ";\n";
      }
    }

    out << output::glsl::indent{indent} << "return;\n";
  }};
}

output::PrintLambda ReturnStatement::to_pretty_glsl(const decl::FragmentDeclaration& fragment,
                                                    uint32_t                         indent) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << output::glsl::indent{indent} << fragment.return_type()->name()
        << " _ = " << expr_->to_glsl() << ";\n";

    const util::memory::Ref<type::StructType> return_struct_type = fragment.return_type();
    for (auto& prop : return_struct_type->properties()) {
      if (prop.index < 0) {
        // Skip properties that don't have an output index.
        continue;
      }

      out << output::glsl::indent{indent}
          << output::glsl::fragment_output_name{static_cast<uint32_t>(prop.index), prop.name}
          << " = _." << prop.name << ";\n";
    }

    out << output::glsl::indent{indent} << "return;\n";
  }};
}

output::PrintLambda ReturnStatement::to_metal(const decl::VertexDeclaration& vertex,
                                              uint32_t                       indent) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << output::glsl::indent{indent} << "return " << expr_->to_metal() << ";\n";
  }};
}

output::PrintLambda ReturnStatement::to_metal(const decl::FragmentDeclaration& fragment,
                                              uint32_t                         indent) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << output::glsl::indent{indent} << "return " << expr_->to_metal() << ";\n";
  }};
}

}  // namespace crystal::compiler::ast::stmt
