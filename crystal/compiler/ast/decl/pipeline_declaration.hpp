#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "crystal/common/pipeline_desc.hpp"
#include "crystal/common/proto/proto.hpp"
#include "crystal/compiler/ast/decl/declaration.hpp"
#include "util/memory/ref_count.hpp"

namespace crystal::compiler::ast {

class Module;

}  // namespace crystal::compiler::ast

namespace crystal::compiler::ast::type {

class Type;

}  // namespace crystal::compiler::ast::type

namespace crystal::compiler::ast::decl {

class VertexDeclaration;
class FragmentDeclaration;

struct PipelineSettings {
  util::memory::Ref<VertexDeclaration>                                          vertex_function;
  util::memory::Ref<FragmentDeclaration>                                        fragment_function;
  std::vector<std::tuple<util::memory::Ref<type::Type>, std::string, uint32_t>> uniforms;
  std::vector<std::tuple<util::memory::Ref<type::Type>, std::string, uint32_t>> textures;

  CullMode   cull_mode         = CullMode::None;
  Winding    winding           = Winding::Clockwise;
  DepthTest  depth_test        = DepthTest::Always;
  DepthWrite depth_write       = DepthWrite::Disable;
  float      depth_bias        = 0.0f;
  float      depth_slope_scale = 0.0f;
  AlphaBlend blend_src         = AlphaBlend::One;
  AlphaBlend blend_dst         = AlphaBlend::Zero;

  void set_property(const std::string_view name, const std::string_view value);
  void set_property(const std::string_view name, const float value);
  void set_property(const std::string_view name, const bool value);
};

class PipelineDeclaration : public Declaration {
  util::memory::Ref<VertexDeclaration>                                          vertex_function_;
  util::memory::Ref<FragmentDeclaration>                                        fragment_function_;
  std::vector<std::tuple<util::memory::Ref<type::Type>, std::string, uint32_t>> uniforms_;
  std::vector<std::tuple<util::memory::Ref<type::Type>, std::string, uint32_t>> textures_;

  CullMode   cull_mode_;
  Winding    winding_;
  DepthTest  depth_test_;
  DepthWrite depth_write_;
  float      depth_bias_;
  float      depth_slope_scale_;
  AlphaBlend blend_src_;
  AlphaBlend blend_dst_;

public:
  PipelineDeclaration(std::string_view name, const PipelineSettings& settings);

  virtual ~PipelineDeclaration() = default;

  [[nodiscard]] const util::memory::Ref<VertexDeclaration>& vertex_function() const {
    return vertex_function_;
  }
  [[nodiscard]] const util::memory::Ref<FragmentDeclaration>& fragment_function() const {
    return fragment_function_;
  }

  [[nodiscard]] const std::vector<std::tuple<util::memory::Ref<type::Type>, std::string, uint32_t>>&
  uniforms() const {
    return uniforms_;
  }
  [[nodiscard]] const std::vector<std::tuple<util::memory::Ref<type::Type>, std::string, uint32_t>>&
  textures() const {
    return textures_;
  }

  void to_cpphdr(std::ostream& out, const Module& mod) const;
  void to_metal(std::ostream& out, const Module& mod) const;

  void make_opengl_crystallib(crystal::common::proto::GLPipeline& pipeline_pb,
                              const Module&                       mod) const;
};

}  // namespace crystal::compiler::ast::decl
