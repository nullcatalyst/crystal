#include "crystal/compiler/ast/stmt/assignment_statement.hpp"

#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/print.hpp"
#include "util/msg/msg.hpp"

namespace crystal::compiler::ast::stmt {

output::PrintLambda AssignmentStatement::to_glsl(uint32_t indent) const {
  switch (op_) {
    case AssignmentOp::Set:
      return output::PrintLambda{[indent, var = var_, value = value_](std::ostream& out) {
        out << output::glsl_indent{indent} << var->to_glsl() << " = " << value->to_glsl() << ";\n";
      }};

    default:
      util::msg::fatal("unhandled assignment operator [", static_cast<uint32_t>(op_), "]");
      break;
  }
}

}  // namespace crystal::compiler::ast::stmt
