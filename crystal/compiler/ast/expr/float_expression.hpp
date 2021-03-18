#pragma once

#include <cstdio>

#include "crystal/compiler/ast/expr/expression.hpp"
#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class FloatExpression : public Expression {
  double value_;

public:
  FloatExpression(double value) : value_(value) {}

  virtual ~FloatExpression() = default;

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) {
      char f_buffer[20] = {0};
      char g_buffer[20] = {0};
      sprintf(f_buffer, "%.1f", value_);
      sprintf(g_buffer, "%g", value_);

      // Always print at least one decimal place. This is required as not all clients' glsl parsers
      // will correctly treat it as a floating point otherwise.
      if (strlen(g_buffer) < strlen(f_buffer)) {
        out << f_buffer;
      } else {
        out << g_buffer;
      }
    }};
  }

  virtual output::PrintLambda to_metal(const output::metal::Options opts) const override {
    return output::PrintLambda{[=](std::ostream& out) {
      char f_buffer[20] = {0};
      char g_buffer[20] = {0};
      sprintf(f_buffer, "%.1f", value_);
      sprintf(g_buffer, "%g", value_);

      // Always print at least one decimal place. This is to prevent a possible edge case where
      // integer division could cause a different result than expected.
      if (strlen(g_buffer) < strlen(f_buffer)) {
        out << f_buffer;
      } else {
        out << g_buffer;
      }
    }};
  }
};

}  // namespace crystal::compiler::ast::expr
