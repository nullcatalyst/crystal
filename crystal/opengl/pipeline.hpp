#pragma once

#include <array>
#include <cstdint>

#include "crystal/common/pipeline_desc.hpp"
#include "crystal/config.hpp"
#include "crystal/opengl/gl.hpp"
#include "crystal/opengl/library.hpp"

namespace crystal::opengl {

class Context;
class CommandBuffer;
class Library;
class RenderPass;

class Pipeline {
  struct Binding {
    bool         active        = false;
    uint32_t     offset        = 0;
    uint32_t     buffer_index  = 0;
    uint32_t     stride        = 0;
    StepFunction step_function = StepFunction::PerVertex;
  };

  static uint32_t next_id_;

  Context*                                   ctx_               = nullptr;
  uint32_t                                   id_                = 0;
  GLuint                                     program_           = 0;
  CullMode                                   cull_mode_         = CullMode::None;
  DepthTest                                  depth_test_        = DepthTest::Never;
  DepthWrite                                 depth_write_       = DepthWrite::Disable;
  float                                      depth_bias_        = 0.0f;
  float                                      depth_slope_scale_ = 0.0f;
  AlphaBlend                                 blend_src_         = AlphaBlend::Zero;
  AlphaBlend                                 blend_dst_         = AlphaBlend::Zero;
  std::array<Binding, MAX_VERTEX_ATTRIBUTES> attributes_        = {};
  std::array<GLuint, MAX_UNIFORM_BINDINGS>   uniforms_          = {};
  std::array<GLuint, MAX_TEXTURE_BINDINGS>   textures_          = {};

public:
  constexpr Pipeline() = default;

  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  Pipeline(Pipeline&& other);
  Pipeline& operator=(Pipeline&& other);

  ~Pipeline();

  void destroy() noexcept;

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::CommandBuffer;

  Pipeline(Context& ctx, Library& library, const PipelineDesc& desc);
};

}  // namespace crystal::opengl
