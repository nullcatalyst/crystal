#include "crystal/compiler/ast/decl/pipeline_declaration.hpp"

#include <tuple>

#include "absl/container/flat_hash_set.h"
#include "crystal/compiler/ast/decl/fragment_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

PipelineDeclaration::PipelineDeclaration(std::string_view name, const PipelineSettings& settings)
    : Declaration(name),
      vertex_function_(settings.vertex_function),
      fragment_function_(settings.fragment_function) {
  if (vertex_function_ != nullptr) {
    vertex_function_->set_name(std::string(name) + "_vert");
  }

  if (fragment_function_ != nullptr) {
    fragment_function_->set_name(std::string(name) + "_frag");
  }
}

void PipelineDeclaration::to_cpphdr(std::ostream& out, const Module& mod) const {
  // Iterate over the vertex/fragment function inputs and precalculate all of the necessary
  // information.
  absl::flat_hash_set<uint32_t> uniforms;
  absl::flat_hash_set<std::tuple<std::string /* type */, std::string /* prop */,
                                 uint32_t /* attr */, uint32_t /* buffer_index */>>
      vertex_attributes;
  absl::flat_hash_set<
      std::tuple<std::string /* type */, uint32_t /* buffer_index */, bool /* instanced */>>
      vertex_buffers;
  if (vertex_function_ != nullptr) {
    for (const auto input : vertex_function_->inputs()) {
      if (input.input_type == VertexInputType::Vertex ||
          input.input_type == VertexInputType::Instanced) {
        const util::memory::Ref<type::StructType> struct_type = input.type;
        for (auto& prop : struct_type->properties()) {
          if (prop.index < 0) {
            continue;
          }
          vertex_attributes.emplace(
              std::make_tuple(input.type->name(), prop.name, prop.index, input.index));
        }

        vertex_buffers.emplace(std::make_tuple(input.type->name(), input.index,
                                               input.input_type == VertexInputType::Instanced));
      }

      if (input.input_type == VertexInputType::Uniform) {
        uniforms.emplace(input.index);
      }
    }
  }
  if (fragment_function_ != nullptr) {
    for (const auto input : fragment_function_->inputs()) {
      if (input.input_type != FragmentInputType::Uniform) {
        continue;
      }
      uniforms.emplace(input.index);
    }
  }

  // clang-format off
  out << "constexpr crystal::PipelineDesc " << name() << "_desc{\n"
      << "    /* .name              = */ \"" << name() << "\",\n"
      << "    /* .cull_mode         = */ crystal::CullMode::Back,\n"
      << "    /* .winding           = */ crystal::Winding::CounterClockwise,\n"
      << "    /* .depth_test        = */ crystal::DepthTest::Always,\n"
      << "    /* .depth_write       = */ crystal::DepthWrite::Disable,\n"
      << "    /* .blend_src         = */ crystal::AlphaBlend::SrcAlpha,\n"
      << "    /* .blend_dst         = */ crystal::AlphaBlend::OneMinusSrcAlpha,\n"
      << "    /* .uniform_bindings  = */";

      // /* .cull_mode         = */ crystal::CullMode::Back,
      // /* .winding           = */ crystal::Winding::CounterClockwise,
      // /* .depth_test        = */ crystal::DepthTest::Always,
      // /* .depth_write       = */ crystal::DepthWrite::Disable,
      // /* .blend_src         = */ crystal::AlphaBlend::SrcAlpha,
      // /* .blend_dst         = */ crystal::AlphaBlend::OneMinusSrcAlpha,

  if (uniforms.size() == 0) {
    out << " {},\n";
  } else {
    out << "\n    {\n";
    // TODO: Sort these before outputting them, for my own sanity.
    for (const auto uniform : uniforms) {
      out << "        crystal::UniformBinding{/* .binding = */ " << uniform << "},\n";
    }
    out << "    },\n";
  }

  // TODO: Support textures.
  out << "    /* .texture_bindings  = */ {},\n";

  out << "    /* .vertex_attributes = */";
  if (vertex_attributes.size() == 0) {
    out << " {},\n";
  } else {
    out << "\n    {\n";
    // TODO: Sort these before outputting them, for my own sanity.
    for (const auto [type, prop, attr, buffer_index] : vertex_attributes) {
      out << "        crystal::VertexAttributeDesc{\n"
          << "            /* .attribute    = */ " << attr << ",\n"
          << "            /* .offset       = */ offsetof(" << type << ", " << prop << "),\n"
          << "            /* .buffer_index = */ " << buffer_index << ",\n"
          << "        },\n";
    }
    out << "    },\n";
  }

  out << "    /* .vertex_buffers    = */";
  if (vertex_buffers.size() == 0) {
    out << " {},\n";
  } else {
    out << "\n    {\n";
    // TODO: Sort these before outputting them, for my own sanity.
    for (const auto [type, buffer_index, instanced] : vertex_buffers) {
      out << "        crystal::VertexBufferDesc{\n"
          << "            /* .buffer_index  = */ " << buffer_index << ",\n"
          << "            /* .stride        = */ sizeof(" << type << "),\n"
          << "            /*. step_function = */ crystal::StepFunction::" << (instanced ? "PerInstance" : "PerVertex") << ",\n"
          << "        },\n";
    }
    out << "    },\n";
  }

  out << "};\n\n";
  // clang-format on
}

void PipelineDeclaration::to_crystallib(crystal::common::proto::Pipeline& pipeline_pb,
                                        const Module&                     mod) const {
  pipeline_pb.set_name(name());

  crystal::common::proto::OpenGL* opengl_pb = pipeline_pb.mutable_opengl();
  {
    std::ostringstream out;
    vertex_function_->to_glsl(out, mod);
    // opengl_pb->set_vertex_source(out.str());

    opengl_pb->set_vertex_source(R"(#version 410 core

// layout(set = 0, binding = 0) uniform Uniform { mat4 u_matrix; };

// Vertex
layout(location = 0) in vec4 i_position;
layout(location = 1) in vec4 i_color;

// Varyings
layout(location = 0) out vec4 v_color;

out gl_PerVertex { vec4 gl_Position; };

void main() {
  // gl_Position = u_matrix * i_position;
  gl_Position = i_position;
  v_color     = i_color;
}
)");
  }

  {
    std::ostringstream out;
    fragment_function_->to_glsl(out, mod);
    // opengl_pb->set_fragment_source(out.str());

    opengl_pb->set_fragment_source(R"(#version 410 core

layout(location = 0) in vec4 v_color;

layout(location = 0) out vec4 o_color;

void main() { o_color = vec4(1.0); }
)");
  }
}

}  // namespace crystal::compiler::ast::decl
