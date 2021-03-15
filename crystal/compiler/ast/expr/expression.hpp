#pragma once

#include "crystal/compiler/ast/output/print.hpp"

namespace crystal::compiler::ast::expr {

class Expression {
public:
  virtual ~Expression() = default;

  virtual output::PrintLambda to_glsl() const { return output::PrintLambda{}; }
};

}  // namespace crystal::compiler::ast::expr
