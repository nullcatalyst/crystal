#include "crystal/compiler/ast/expr/call_expression.hpp"

namespace crystal::compiler::ast::expr {

output::PrintLambda CallExpression::to_glsl(const output::glsl::Options opts) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << function_->to_glsl(opts) << "(";
    bool first = true;
    for (const auto& arg : arguments_) {
      if (first) {
        first = false;
      } else {
        out << ", ";
      }
      out << arg->to_glsl(opts);
    }
    out << ")";
  }};
}

output::PrintLambda CallExpression::to_metal(const output::metal::Options opts) const {
  return output::PrintLambda{[=](std::ostream& out) {
    out << function_->to_metal(opts) << "(";
    bool first = true;
    for (const auto& arg : arguments_) {
      if (first) {
        first = false;
      } else {
        out << ", ";
      }
      out << arg->to_metal(opts);
    }
    out << ")";
  }};
}

}  // namespace crystal::compiler::ast::expr
