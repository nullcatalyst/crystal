#pragma once

#include <iostream>
#include <string_view>

namespace crystal::compiler::ast::output {

struct glsl_indent {
  uint32_t indent;
};

inline std::ostream& operator<<(std::ostream& out, glsl_indent op) {
  for (int i = 0; i < op.indent; ++i) {
    out << "    ";
  }
  return out;
}

struct glsl_mangle_name {
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, glsl_mangle_name op) {
  return out << "_" << op.name;
}

}  // namespace crystal::compiler::ast::output
