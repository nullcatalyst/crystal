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

void PipelineSettings::set_property(const std::string_view name, const std::string_view value) {
  if (name == "cull") {
    if (value == "back") {
      cull_mode = CullMode::Back;
    } else if (value == "front") {
      cull_mode = CullMode::Front;
    } else if (value == "none") {
      cull_mode = CullMode::None;
    } else {
      util::msg::fatal("setting [cull] to unknown value [", value, "]");
    }
  } else if (name == "winding") {
    if (value == "cw" || value == "clockwise") {
      winding = Winding::Clockwise;
    } else if (value == "ccw" || value == "counterclockwise") {
      winding = Winding::CounterClockwise;
    } else {
      util::msg::fatal("setting [winding] to unknown value [", value, "]");
    }
  } else if (name == "depth_test") {
    if (value == "never") {
      depth_test = DepthTest::Never;
    } else if (value == "less") {
      depth_test = DepthTest::Less;
    } else if (value == "equal") {
      depth_test = DepthTest::Equal;
    } else if (value == "lequal") {
      depth_test = DepthTest::LessEqual;
    } else if (value == "greater") {
      depth_test = DepthTest::Greater;
    } else if (value == "nequal") {
      depth_test = DepthTest::NotEqual;
    } else if (value == "gequal") {
      depth_test = DepthTest::GreaterEqual;
    } else if (value == "always") {
      depth_test = DepthTest::Always;
    } else {
      util::msg::fatal("setting [depth_test] to unknown value [", value, "]");
    }
  } else if (name == "blend_src") {
    if (value == "zero") {
      blend_src = AlphaBlend::Zero;
    } else if (value == "one") {
      blend_src = AlphaBlend::One;
    } else if (value == "src_color") {
      blend_src = AlphaBlend::SrcColor;
    } else if (value == "one_minus_src_color") {
      blend_src = AlphaBlend::OneMinusSrcColor;
    } else if (value == "dst_color") {
      blend_src = AlphaBlend::DstColor;
    } else if (value == "one_minus_dst_color") {
      blend_src = AlphaBlend::OneMinusDstColor;
    } else if (value == "src_alpha") {
      blend_src = AlphaBlend::SrcAlpha;
    } else if (value == "one_minus_src_alpha") {
      blend_src = AlphaBlend::OneMinusSrcAlpha;
    } else if (value == "dst_alpha") {
      blend_src = AlphaBlend::DstAlpha;
    } else if (value == "one_minus_dst_alpha") {
      blend_src = AlphaBlend::OneMinusDstAlpha;
    } else {
      util::msg::fatal("setting [blend_src] to unknown value [", value, "]");
    }
  } else if (name == "blend_dst") {
    if (value == "zero") {
      blend_dst = AlphaBlend::Zero;
    } else if (value == "one") {
      blend_dst = AlphaBlend::One;
    } else if (value == "src_color") {
      blend_dst = AlphaBlend::SrcColor;
    } else if (value == "one_minus_src_color") {
      blend_dst = AlphaBlend::OneMinusSrcColor;
    } else if (value == "dst_color") {
      blend_dst = AlphaBlend::DstColor;
    } else if (value == "one_minus_dst_color") {
      blend_dst = AlphaBlend::OneMinusDstColor;
    } else if (value == "src_alpha") {
      blend_dst = AlphaBlend::SrcAlpha;
    } else if (value == "one_minus_src_alpha") {
      blend_dst = AlphaBlend::OneMinusSrcAlpha;
    } else if (value == "dst_alpha") {
      blend_dst = AlphaBlend::DstAlpha;
    } else if (value == "one_minus_dst_alpha") {
      blend_dst = AlphaBlend::OneMinusDstAlpha;
    } else {
      util::msg::fatal("setting [blend_dst] to unknown value [", value, "]");
    }
  }
}

void PipelineSettings::set_property(const std::string_view name, const bool value) {
  if (name == "depth_write") {
    depth_write = (value ? DepthWrite::Enable : DepthWrite::Disable);
  }
}

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

  cull_mode_   = settings.cull_mode;
  winding_     = settings.winding;
  depth_test_  = settings.depth_test;
  depth_write_ = settings.depth_write;
  blend_src_   = settings.blend_src;
  blend_dst_   = settings.blend_dst;
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

  std::string winding_out;
  switch (winding_) {
    case Winding::Clockwise:
      winding_out = "Clockwise";
      break;
    case Winding::CounterClockwise:
      winding_out = "CounterClockwise";
      break;
  }

  std::string depth_test_out;
  switch (depth_test_) {
    case DepthTest::Never:
      depth_test_out = "Never";
      break;
    case DepthTest::Less:
      depth_test_out = "Less";
      break;
    case DepthTest::Equal:
      depth_test_out = "Equal";
      break;
    case DepthTest::LessEqual:
      depth_test_out = "LessEqual";
      break;
    case DepthTest::Greater:
      depth_test_out = "Greater";
      break;
    case DepthTest::NotEqual:
      depth_test_out = "NotEqual";
      break;
    case DepthTest::GreaterEqual:
      depth_test_out = "GreaterEqual";
      break;
    case DepthTest::Always:
      depth_test_out = "Always";
      break;
  }

  std::string depth_write_out;
  switch (depth_write_) {
    case DepthWrite::Disable:
      depth_write_out = "Disable";
      break;
    case DepthWrite::Enable:
      depth_write_out = "Enable";
      break;
  }

  std::string blend_src_out = "One";
  std::string blend_dst_out = "Zero";

  out << "const crystal::PipelineDesc " << name() << "_desc{\n"
      << "    /* .name              = */ \"" << name() << "\",\n"
      << "    /* .cull_mode         = */ crystal::CullMode::" << cull_out << ",\n"
      << "    /* .winding           = */ crystal::Winding::" << winding_out << ",\n"
      << "    /* .depth_test        = */ crystal::DepthTest::" << depth_test_out << ",\n"
      << "    /* .depth_write       = */ crystal::DepthWrite::" << depth_write_out << ",\n"
      << "    /* .blend_src         = */ crystal::AlphaBlend::" << blend_src_out << ",\n"
      << "    /* .blend_dst         = */ crystal::AlphaBlend::" << blend_dst_out << ",\n";

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
          << "            /*. step_function = */ crystal::StepFunction::"
          << (instanced ? "PerInstance" : "PerVertex") << ",\n"
          << "        },\n";
    }
    out << "    },\n";
  }

  out << "};\n\n";
}

void PipelineDeclaration::to_metal(std::ostream& out, const Module& mod) const {
  vertex_function_->to_metal(out, mod, *this);

  if (fragment_function_ != nullptr) {
    fragment_function_->to_metal(out, mod, *this);
  }
}

void PipelineDeclaration::make_opengl_crystallib(crystal::common::proto::GLPipeline& pipeline_pb,
                                                 const Module&                       mod) const {
  pipeline_pb.set_name(name());

  {  // Vertex shader.
    std::ostringstream out;
    vertex_function_->to_glsl(out, mod, false, false);
    pipeline_pb.set_vertex_source(out.str());
    // std::cout << "vertex=\n" << out.str() << std::endl;
  }

  {  // Fragment shader.
    std::ostringstream out;
    fragment_function_->to_glsl(out, mod, false, false);
    pipeline_pb.set_fragment_source(out.str());
    // std::cout << "fragment=\n" << out.str() << std::endl;
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
