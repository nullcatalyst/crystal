#pragma once

#include <array>
#include <initializer_list>
#include <tuple>

#include "crystal/common/render_pass_desc.hpp"
#include "crystal/opengl/gl.hpp"

namespace crystal::opengl {

class Context;
class CommandBuffer;
class Pipeline;
class Texture;

class RenderPass {
  Context*              ctx_              = nullptr;
  GLuint                framebuffer_      = 0;
  uint32_t              width_            = 0;
  uint32_t              height_           = 0;
  uint32_t              attachment_count_ = 0;
  std::array<GLuint, 8> attachments_      = {};

  // TODO: Add support for clear values here.

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
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::CommandBuffer;
  friend class ::crystal::opengl::Pipeline;

  RenderPass(Context& ctx);
  RenderPass(
      Context&                                                                     ctx,
      const std::initializer_list<std::tuple<const Texture&, ColorAttachmentDesc>> color_textures);
  RenderPass(
      Context&                                                                     ctx,
      const std::initializer_list<std::tuple<const Texture&, ColorAttachmentDesc>> color_textures,
      const std::tuple<const Texture&, DepthAttachmentDesc>                        depth_texture);
};

}  // namespace crystal::opengl
