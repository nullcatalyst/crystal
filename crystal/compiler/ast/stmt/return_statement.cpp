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

output::PrintLambda ReturnStatement::to_glsl(const output::glsl::Options opts) const {
  if (opts.vertex != nullptr) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl::indent{opts.indent} << opts.vertex->return_type()->name() << " _"
          << (opts.pretty ? " = " : "=") << expr_->to_glsl(opts) << (opts.pretty ? ";\n" : ";");

      const util::memory::Ref<type::StructType> return_struct_type = opts.vertex->return_type();
      for (auto& prop : return_struct_type->properties()) {
        if (prop.index < 0) {
          // Skip properties that don't have an output index.
          continue;
        }

        out << output::glsl::indent{opts.indent}
            << output::glsl::varying_name{static_cast<uint32_t>(prop.index), prop.name}
            << (opts.pretty ? " = " : "=") << "_." << prop.name << (opts.pretty ? ";\n" : ";");

        if (prop.index == 0) {
          // The zero output for the vertex function must be the vertex position.
          out << output::glsl::indent{opts.indent} << "gl_Position" << (opts.pretty ? " = " : "=")
              << "_." << prop.name << (opts.pretty ? ";\n" : ";");
        }
      }

      out << output::glsl::indent{opts.indent} << (opts.pretty ? "return;\n" : "return;");
    }};
  }

  if (opts.fragment != nullptr) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl::indent{opts.indent} << opts.fragment->return_type()->name()
          << (opts.pretty ? " _ = " : " _=") << expr_->to_glsl(opts) << (opts.pretty ? ";\n" : ";");

      const util::memory::Ref<type::StructType> return_struct_type = opts.fragment->return_type();
      for (auto& prop : return_struct_type->properties()) {
        if (prop.index < 0) {
          // Skip properties that don't have an output index.
          continue;
        }

        out << output::glsl::indent{opts.indent}
            << output::glsl::fragment_output_name{static_cast<uint32_t>(prop.index), prop.name}
            << (opts.pretty ? " = " : "=") << "_." << prop.name << (opts.pretty ? ";\n" : ";");
      }

      out << output::glsl::indent{opts.indent} << (opts.pretty ? "return;\n" : "return;");
    }};
  }

  return output::PrintLambda{[=](std::ostream& out) {}};
}

output::PrintLambda ReturnStatement::to_metal(const output::metal::Options opts) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << output::metal::indent{opts.indent} << "return " << expr_->to_metal(opts) << ";\n";
  }};
}

}  // namespace crystal::compiler::ast::stmt
