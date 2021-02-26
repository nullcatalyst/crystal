#pragma once

#include <array>

#include "crystal/common/render_pass_desc.hpp"
#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class CommandBuffer;
class Pipeline;

class RenderPass {
  OBJC(MTLRenderPassDescriptor) render_pass_desc_ = nullptr;
  uint32_t                      attachment_count_ = 0;
  std::array<MTLPixelFormat, 5> pixel_formats_    = {};
  uint32_t                      width_            = 0;
  uint32_t                      height_           = 0;

public:
  constexpr RenderPass() = default;

  RenderPass(const RenderPass&) = delete;
  RenderPass& operator=(const RenderPass&) = delete;

  RenderPass(RenderPass&& other);
  RenderPass& operator=(RenderPass&& other);

  ~RenderPass();

  [[nodiscard]] constexpr uint32_t width() const { return width_; }
  [[nodiscard]] constexpr uint32_t height() const { return height_; }

  void destroy() noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::CommandBuffer;
  friend class ::crystal::metal::Pipeline;

  RenderPass(Context& ctx);
  // RenderPass(Context& ctx, const RenderPassDesc& desc);
};

}  // namespace crystal::metal
