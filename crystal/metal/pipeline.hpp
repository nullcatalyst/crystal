#pragma once

#include <array>
#include <cstdint>

#include "absl/container/inlined_vector.h"
#include "crystal/common/pipeline_desc.hpp"
#include "crystal/config.hpp"
#include "crystal/metal/library.hpp"
#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class CommandBuffer;
class Library;
class RenderPass;

class Pipeline {
  OBJC(MTLRenderPipelineState) render_pipeline_        = nullptr;
  OBJC(MTLDepthStencilState) depth_stencil_state_      = nullptr;
  MTLCullMode                               cull_mode_ = {};
  MTLWinding                                winding_   = {};
  std::array<uint32_t, MAX_UNIFORM_BUFFERS> uniforms_  = {};

public:
  constexpr Pipeline() = default;

  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  Pipeline(Pipeline&& other);
  Pipeline& operator=(Pipeline&& other);

  ~Pipeline();

  void destroy() noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::CommandBuffer;

  Pipeline(OBJC(MTLDevice) device, Library& library, RenderPass& render_pass,
           const PipelineDesc& desc);
};

}  // namespace crystal::metal
