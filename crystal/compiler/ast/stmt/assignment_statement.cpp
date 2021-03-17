#include "crystal/compiler/ast/stmt/assignment_statement.hpp"

#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast::stmt {

output::PrintLambda AssignmentStatement::to_glsl(const output::glsl::Options opts) const {
  switch (op_) {
    case AssignmentOp::Set:
      return output::PrintLambda{[=](std::ostream& out) {
        out << output::glsl::indent{opts.indent} << var_->to_glsl(opts)
            << (opts.pretty ? " = " : "=") << value_->to_glsl(opts) << (opts.pretty ? ";\n" : ";");
      }};

    default:
      util::msg::fatal("unhandled assignment operator [", static_cast<uint32_t>(op_), "]");
      break;
  }
}

output::PrintLambda AssignmentStatement::to_metal(const output::metal::Options opts) const {
  switch (op_) {
    case AssignmentOp::Set:
      return output::PrintLambda{[=](std::ostream& out) {
        out << output::metal::indent{opts.indent} << var_->to_metal(opts) << " = "
            << value_->to_metal(opts) << ";\n";
      }};

    default:
      util::msg::fatal("unhandled assignment operator [", static_cast<uint32_t>(op_), "]");
      break;
  }
}

}  // namespace crystal::compiler::ast::stmt
