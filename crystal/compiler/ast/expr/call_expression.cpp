#include "crystal/compiler/ast/expr/call_expression.hpp"

#include "crystal/compiler/ast/expr/identifier_expression.hpp"
#include "crystal/compiler/ast/module.hpp"

namespace crystal::compiler::ast::expr {

output::PrintLambda CallExpression::to_glsl(const output::glsl::Options opts) const {
  const auto type = opts.mod.find_type(name_);
  if (type.has_value()) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << type.value()->glsl_name() << "(";
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

  return output::PrintLambda{[=](std::ostream& out) {
    out << name_ << "(";
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
  const auto type = opts.mod.find_type(name_);
  if (type.has_value()) {
    return output::PrintLambda{[=](std::ostream& out) {
      out << type.value()->metal_name() << "(";
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

  return output::PrintLambda{[=](std::ostream& out) {
    out << name_ << "(";
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
