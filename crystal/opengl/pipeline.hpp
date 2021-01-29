#pragma once

#include <cstdint>

#include "absl/container/inlined_vector.h"
#include "crystal/common/pipeline_desc.hpp"
#include "crystal/opengl/gl.hpp"
#include "crystal/opengl/library.hpp"

namespace crystal::opengl {

class Context;
class CommandBuffer;
class Library;
class RenderPass;

class Pipeline {
  struct Binding {
    uint32_t     id;
    uint32_t     offset;
    uint32_t     buffer_index;
    uint32_t     stride;
    StepFunction step_function;
  };

  static uint32_t next_id_;

  Context*                         ctx_         = nullptr;
  uint32_t                         id_          = 0;
  GLuint                           program_     = 0;
  CullMode                         cull_mode_   = CullMode::None;
  DepthTest                        depth_test_  = DepthTest::Never;
  DepthWrite                       depth_write_ = DepthWrite::Disable;
  AlphaBlend                       blend_src_   = AlphaBlend::Zero;
  AlphaBlend                       blend_dst_   = AlphaBlend::Zero;
  absl::InlinedVector<Binding, 16> bindings_    = {};

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
