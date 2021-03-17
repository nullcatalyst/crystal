#include "crystal/compiler/ast/stmt/variable_statement.hpp"

#include "crystal/compiler/ast/decl/vertex_declaration.hpp"

namespace crystal::compiler::ast::stmt {

output::PrintLambda VariableStatement::to_glsl(const output::glsl::Options opts) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << output::glsl::indent{opts.indent} << type_->glsl_name() << " "
        << output::glsl::mangle_name{name_} << (opts.pretty ? ";\n" : ";");
  }};
}

output::PrintLambda VariableStatement::to_metal(const output::metal::Options opts) const {
  if (opts.vertex != nullptr && type_ == opts.vertex->return_type()) {
    std::string short_name = opts.vertex->name().substr(0, opts.vertex->name().size() - 5);
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::metal::indent{opts.indent} << short_name << "_v "
          << output::metal::mangle_name{name_} << ";\n";
    }};
  }

  return output::PrintLambda{[=](std::ostream& out) {
    out << output::metal::indent{opts.indent} << type_->metal_name() << " "
        << output::metal::mangle_name{name_} << ";\n";
  }};
}

}  // namespace crystal::compiler::ast::stmt
