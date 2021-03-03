#pragma once

#include <array>

#include "crystal/common/render_pass_desc.hpp"
#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class CommandBuffer;
class Pipeline;
class Texture;

class RenderPass {
  OBJC(MTLRenderPassDescriptor) render_pass_desc_    = nullptr;
  uint32_t                      width_               = 0;
  uint32_t                      height_              = 0;
  uint32_t                      color_count_         = 0;
  std::array<MTLPixelFormat, 4> color_pixel_formats_ = {};
  bool                          has_depth_           = false;
  MTLPixelFormat                depth_pixel_format_  = {};

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

  RenderPass(Context& ctx, MTLPixelFormat pixel_format);
  RenderPass(
      const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures);
  RenderPass(const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
             const std::tuple<const Texture&, AttachmentDesc>                        depth_texture);
};

}  // namespace crystal::metal
