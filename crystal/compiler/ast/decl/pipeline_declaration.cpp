#include "crystal/compiler/ast/decl/pipeline_declaration.hpp"

#include <sstream>
#include <tuple>

#include "absl/container/flat_hash_set.h"
#include "crystal/compiler/ast/decl/fragment_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/output/metal.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

PipelineDeclaration::PipelineDeclaration(std::string_view name, const PipelineSettings& settings)
    : Declaration(name),
      vertex_function_(settings.vertex_function),
      fragment_function_(settings.fragment_function),
      uniforms_(settings.uniforms),
      textures_(settings.textures) {
  if (vertex_function_ != nullptr) {
    vertex_function_->set_name(std::string(name) + "_vert");
    for (const auto& [type, name, index] : settings.uniforms) {
      vertex_function_->add_uniform(type, name, index);
    }
  }

  if (fragment_function_ != nullptr) {
    fragment_function_->set_name(std::string(name) + "_frag");
    for (const auto& [type, name, index] : settings.uniforms) {
      fragment_function_->add_uniform(type, name, index);
    }
    for (const auto& [type, name, index] : settings.textures) {
      fragment_function_->add_texture(type, name, index);
    }
  }

  if (settings.cull == "back") {
    cull_mode_ = CullMode::Back;
  } else if (settings.cull == "front") {
    cull_mode_ = CullMode::Front;
  } else {
    cull_mode_ = CullMode::None;
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
    for (const auto& input : vertex_function_->inputs()) {
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
    for (const auto& input : fragment_function_->inputs()) {
      if (input.input_type != FragmentInputType::Uniform) {
        continue;
      }
      uniforms.emplace(input.index);
    }
  }

  std::string cull_out;
  switch (cull_mode_) {
    case CullMode::None:
      cull_out = "None";
      break;
    case CullMode::Back:
      cull_out = "Back";
      break;
    case CullMode::Front:
      cull_out = "Front";
      break;
  }

  // clang-format off
  out << "const crystal::PipelineDesc " << name() << "_desc{\n"
      << "    /* .name              = */ \"" << name() << "\",\n"
      << "    /* .cull_mode         = */ crystal::CullMode::" << cull_out << ",\n"
      << "    /* .winding           = */ crystal::Winding::CounterClockwise,\n"
      << "    /* .depth_test        = */ crystal::DepthTest::Always,\n"
      << "    /* .depth_write       = */ crystal::DepthWrite::Disable,\n"
      << "    /* .blend_src         = */ crystal::AlphaBlend::One,\n"
      << "    /* .blend_dst         = */ crystal::AlphaBlend::Zero,\n";

  out << "    /* .vertex_attributes = */";
  if (vertex_attributes.size() == 0) {
    out << " {},\n";
  } else {
    out << "\n    {\n";
    // TODO: Sort these before outputting them, for my own sanity.
    for (const auto& [type, prop, attr, buffer_index] : vertex_attributes) {
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
    for (const auto& [type, buffer_index, instanced] : vertex_buffers) {
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

void PipelineDeclaration::to_metal(std::ostream& out, const Module& mod) const {
  vertex_function_->to_metal(out, mod);

  if (fragment_function_ != nullptr) {
    fragment_function_->to_metal(out, mod);
  }
}

void PipelineDeclaration::make_opengl_crystallib(crystal::common::proto::GLPipeline& pipeline_pb,
                                                 const Module&                       mod) const {
  pipeline_pb.set_name(name());

  {  // Vertex shader.
    std::ostringstream out;
    vertex_function_->to_glsl(out, mod, false, false);
    pipeline_pb.set_vertex_source(out.str());
    // std::cout << "vertex =\n" << out.str() << std::endl;
  }

  {  // Fragment shader.
    std::ostringstream out;
    fragment_function_->to_glsl(out, mod, false, false);
    pipeline_pb.set_fragment_source(out.str());
    // std::cout << "fragment =\n" << out.str() << std::endl;
  }

  {  // Uniforms.
    for (const auto& [type, name, binding] : uniforms_) {
      crystal::common::proto::GLUniform* uniform_pb = pipeline_pb.add_uniforms();

      std::stringstream sstream;
      sstream << "U" << binding;
      uniform_pb->set_name(sstream.str());
      uniform_pb->set_binding(binding);
    }
  }
}

}  // namespace crystal::compiler::ast::decl
