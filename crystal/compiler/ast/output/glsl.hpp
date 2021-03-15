#pragma once

#include <iostream>
#include <string_view>

namespace crystal::compiler::ast::output {

constexpr std::string_view VERSION_HDR = "#version 410 core\n";

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

struct glsl_vertex_input_name {
  uint32_t         index;
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, glsl_vertex_input_name op) {
  return out << "i" << op.index << "_" << op.name;
}

struct glsl_varying_name {
  uint32_t         index;
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, glsl_varying_name op) {
  return out << "v" << op.index << "_" << op.name;
}

struct glsl_fragment_output_name {
  uint32_t         index;
  std::string_view name;
};

inline std::ostream& operator<<(std::ostream& out, glsl_fragment_output_name op) {
  return out << "o" << op.index << "_" << op.name;
}

}  // namespace crystal::compiler::ast::output
