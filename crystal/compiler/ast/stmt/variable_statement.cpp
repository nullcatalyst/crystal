#include "crystal/compiler/ast/stmt/variable_statement.hpp"

#include "crystal/compiler/ast/decl/vertex_declaration.hpp"

namespace crystal::compiler::ast::stmt {

output::PrintLambda VariableStatement::to_glsl(const output::glsl::Options opts) const {
  if (expr_ != nullptr) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl::indent{opts.indent} << type_->glsl_name() << " "
          << output::glsl::mangle_name{name_} << (opts.pretty ? " = " : "=") << expr_->to_glsl(opts)
          << (opts.pretty ? ";\n" : ";");
    }};
  } else {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::glsl::indent{opts.indent} << type_->glsl_name() << " "
          << output::glsl::mangle_name{name_} << (opts.pretty ? ";\n" : ";");
    }};
  }
}

output::PrintLambda VariableStatement::to_metal(const output::metal::Options opts) const {
  if (opts.vertex != nullptr && type_ == opts.vertex->return_type()) {
    std::string short_name = opts.vertex->name().substr(0, opts.vertex->name().size() - 5);
    if (expr_ != nullptr) {
      return output::PrintLambda{[=](std::ostream& out) {
        out << output::metal::indent{opts.indent} << short_name << "_v "
            << output::metal::mangle_name{name_} << " = " << expr_->to_metal(opts) << ";\n";
      }};
    } else {
      return output::PrintLambda{[=](std::ostream& out) {
        out << output::metal::indent{opts.indent} << short_name << "_v "
            << output::metal::mangle_name{name_} << ";\n";
      }};
    }
  }

  if (expr_ != nullptr) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::metal::indent{opts.indent} << type_->metal_name() << " "
          << output::metal::mangle_name{name_} << " = " << expr_->to_metal(opts) << ";\n";
    }};
  } else {
    return output::PrintLambda{[=](std::ostream& out) {
      out << output::metal::indent{opts.indent} << type_->metal_name() << " "
          << output::metal::mangle_name{name_} << ";\n";
    }};
  }
}

}  // namespace crystal::compiler::ast::stmt
