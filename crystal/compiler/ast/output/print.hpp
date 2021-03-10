#pragma once

#include <functional>
#include <iostream>

namespace crystal::compiler::ast::output {

struct PrintLambda {
  std::function<void(std::ostream&)> impl;

  PrintLambda() : impl([](std::ostream&) {}) {}

  template <typename T>
  PrintLambda(T&& impl) : impl(std::move(impl)) {}
};

inline std::ostream& operator<<(std::ostream& out, const PrintLambda& lambda) {
  lambda.impl(out);
  return out;
}

}  // namespace crystal::compiler::ast::output
