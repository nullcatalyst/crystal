#pragma once

#include <iostream>
#include <string_view>

namespace crystal::compiler::ast {

class Module;

}  // namespace crystal::compiler::ast

namespace crystal::compiler::ast::decl {

class PipelineDeclaration;
class VertexDeclaration;
class FragmentDeclaration;

}  // namespace crystal::compiler::ast::decl

namespace crystal::compiler::ast::output::metal {

constexpr std::string_view HDR = R"(#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;
)";

struct Options {
  const Module&                    mod;
  const decl::VertexDeclaration*   vertex;
  const decl::FragmentDeclaration* fragment;
  const uint32_t                   indent;

  constexpr Options(const Module& mod, const decl::VertexDeclaration* vertex,
                    const decl::FragmentDeclaration* fragment, const uint32_t indent)
      : mod(mod), vertex(vertex), fragment(fragment), indent(indent) {}

  constexpr Options incr_indent() const { return Options{mod, vertex, fragment, indent}; }
};

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
  return out << "in." << op.name;
}

uint32_t uniform_binding(const decl::PipelineDeclaration& vertex, uint32_t binding);

}  // namespace crystal::compiler::ast::output::metal
