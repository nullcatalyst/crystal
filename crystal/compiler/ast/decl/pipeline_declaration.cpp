#include "crystal/compiler/ast/decl/pipeline_declaration.hpp"

#include <tuple>

#include "absl/container/flat_hash_set.h"
#include "crystal/compiler/ast/decl/fragment_declaration.hpp"
#include "crystal/compiler/ast/decl/vertex_declaration.hpp"
#include "crystal/compiler/ast/module.hpp"
#include "crystal/compiler/ast/type/struct_type.hpp"

namespace crystal::compiler::ast::decl {

PipelineDeclaration::PipelineDeclaration(std::string_view name, const Module& mod,
                                         const PipelineSettings& settings)
    : Declaration(name) {
  if (settings.vertex_name.size() > 0) {
    const auto vertex_function = mod.find_vertex_function(settings.vertex_name);
    if (vertex_function != std::nullopt) {
      vertex_function_ = vertex_function.value();
    }
  }

  if (settings.fragment_name.size() > 0) {
    const auto fragment_function = mod.find_fragment_function(settings.fragment_name);
    if (fragment_function != std::nullopt) {
      fragment_function_ = fragment_function.value();
    }
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
      << "    /* .vertex            = */ " << (vertex_function_ != nullptr ? "\"" + vertex_function_->name() + "\"" : "nullptr") << ",\n"
      << "    /* .fragment          = */ " << (fragment_function_ != nullptr ? "\"" + fragment_function_->name() + "\"" : "nullptr") << ",\n"
      << "    /* .cull_mode         = */ crystal::CullMode::None,\n"
      << "    /* .winding           = */ crystal::Winding::CounterClockwise,\n"
      << "    /* .depth_test        = */ crystal::DepthTest::Always,\n"
      << "    /* .depth_write       = */ crystal::DepthWrite::Disable,\n"
      << "    /* .blend_src         = */ crystal::AlphaBlend::One,\n"
      << "    /* .blend_dst         = */ crystal::AlphaBlend::Zero,\n"
      << "    /* .uniform_bindings  = */";

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

}  // namespace crystal::compiler::ast::decl
