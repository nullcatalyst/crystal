#pragma once

#include <iostream>

namespace crystal::compiler::ast::expr {

class Expression {
public:
  virtual ~Expression() = default;

  virtual void to_glsl(std::ostream& out) {}
};

}  // namespace crystal::compiler::ast::expr
