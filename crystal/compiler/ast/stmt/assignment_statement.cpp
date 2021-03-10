#include "crystal/compiler/ast/stmt/assignment_statement.hpp"

#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast::stmt {

output::PrintLambda AssignmentStatement::to_glsl() const {
  switch (op_) {
    case AssignmentOp::Set:
      return output::PrintLambda{
          [=](std::ostream& out) { out << var_->to_glsl() << "=" << value_->to_glsl() << ";"; }};

    default:
      util::msg::fatal("unhandled assignment operator [", static_cast<uint32_t>(op_), "]");
      break;
  }
}

output::PrintLambda AssignmentStatement::to_pretty_glsl(uint32_t indent) const {
  switch (op_) {
    case AssignmentOp::Set:
      return output::PrintLambda{[=](std::ostream& out) {
        out << output::glsl_indent{indent} << var_->to_glsl() << " = " << value_->to_glsl()
            << ";\n";
      }};

    default:
      util::msg::fatal("unhandled assignment operator [", static_cast<uint32_t>(op_), "]");
      break;
  }
}

}  // namespace crystal::compiler::ast::stmt
