#pragma once

#include <iostream>
#include <string_view>

namespace crystal::compiler::ast::output::glsl {

constexpr std::string_view HDR = "#version 410 core\n";

struct indent {
  uint32_t indent;
};

inline std::ostream& operator<<(std::ostream& out, indent op) {
  for (int i = 0; i < op.indent; ++i) {
    out << "    ";
  }
  return out;
}

struct mangle_name {
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, mangle_name op) { return out << "_" << op.name; }

struct vertex_input_name {
  uint32_t         index;
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, vertex_input_name op) {
  return out << "i" << op.index << "_" << op.name;
}

struct varying_name {
  uint32_t         index;
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, varying_name op) {
  return out << "v" << op.index << "_" << op.name;
}

struct fragment_output_name {
  uint32_t         index;
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, fragment_output_name op) {
  return out << "o" << op.index << "_" << op.name;
}

}  // namespace crystal::compiler::ast::output::glsl
