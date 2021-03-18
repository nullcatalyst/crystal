#pragma once

#include "crystal/compiler/ast/output/glsl.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class Expression {
public:
  virtual ~Expression() = default;

  [[nodiscard]] virtual bool is_identifier() const { return false; }

  virtual output::PrintLambda to_glsl(const output::glsl::Options opts) const   = 0;
  virtual output::PrintLambda to_metal(const output::metal::Options opts) const = 0;
};

}  // namespace crystal::compiler::ast::expr
