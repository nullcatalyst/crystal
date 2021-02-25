#include "crystal/opengl/render_pass.hpp"

#include <array>

#include "crystal/opengl/command_buffer.hpp"
#include "crystal/opengl/context.hpp"

namespace crystal::opengl {

RenderPass::RenderPass(RenderPass&& other)
    : ctx_(other.ctx_),
      framebuffer_(other.framebuffer_),
      width_(other.width_),
      height_(other.height_) {
  other.ctx_         = nullptr;
  other.framebuffer_ = 0;
  other.width_       = 0;
  other.height_      = 0;
}

RenderPass& RenderPass::operator=(RenderPass&& other) {
  destroy();

  ctx_         = other.ctx_;
  framebuffer_ = other.framebuffer_;
  width_       = other.width_;
  height_      = other.height_;

  other.ctx_         = nullptr;
  other.framebuffer_ = 0;
  other.width_       = 0;
  other.height_      = 0;

  return *this;
}

RenderPass::~RenderPass() { destroy(); }

void RenderPass::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  glDeleteFramebuffers(1, &framebuffer_);

  ctx_         = nullptr;
  framebuffer_ = 0;
  width_       = 0;
  height_      = 0;
}

RenderPass::RenderPass(Context& ctx) : ctx_(&ctx) {}

RenderPass::RenderPass(
    Context&                                                                     ctx,
    const std::initializer_list<std::tuple<const Texture&, ColorAttachmentDesc>> color_textures)
    : ctx_(&ctx), framebuffer_(0) {
  if (color_textures.size() == 0) {
    util::msg::fatal("framebuffer must render to at least one texture");
  }

  // TODO: Assert that all textures have the same size.
  // TODO: Assert that there are at most 8 color textures.
  // TODO: Save the clear values.

  GL_ASSERT(glGenFramebuffers(1, &framebuffer_), "generating framebuffer");
  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_), "binding framebuffer");

  std::array<GLenum, 8> draw_buffers;
  for (int i = 0; i < color_textures.size(); ++i) {
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;

    const auto& [texture, desc] = color_textures.begin()[i];
    GL_ASSERT(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.texture_, 0),
              "attaching texture to framebuffer");
  }

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc]     = color_textures.begin()[0];
    width_                          = texture.width_;
    height_                         = texture.height_;
    attachments_[attachment_count_] = texture.texture_;
    ++attachment_count_;
    ctx.retain_texture_(texture.texture_);
  }

  GL_ASSERT(glDrawBuffers(color_textures.size(), draw_buffers.data()),
            "setting draw buffers for framebuffer");

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    util::msg::fatal("framebuffer is not complete");
  }
}

RenderPass::RenderPass(
    Context&                                                                     ctx,
    const std::initializer_list<std::tuple<const Texture&, ColorAttachmentDesc>> color_textures,
    const std::tuple<const Texture&, DepthAttachmentDesc>                        depth_texture)
    : ctx_(&ctx), framebuffer_(0) {
  GL_ASSERT(glGenFramebuffers(1, &framebuffer_), "generating framebuffer");
  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_), "binding framebuffer");

  // TODO: Assert that all textures have the same size.
  // TODO: Assert that there are at most 8 color textures.
  // TODO: Save the clear values.

  std::array<GLenum, 8> draw_buffers;
  for (int i = 0; i < color_textures.size(); ++i) {
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;

    const auto& [texture, desc] = color_textures.begin()[i];
    GL_ASSERT(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.texture_, 0),
              "attaching texture to framebuffer");
  }

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = depth_texture;
    width_                      = texture.width_;
    height_                     = texture.height_;

    GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                     texture.texture_, 0),
              "attaching depth texture to framebuffer");
  }

  if (color_textures.size() == 0) {
    GL_ASSERT(glDrawBuffer(GL_NONE), "setting no draw buffer");
    GL_ASSERT(glReadBuffer(GL_NONE), "setting no read buffer");
  } else {
    GL_ASSERT(glDrawBuffers(color_textures.size(), draw_buffers.data()),
              "setting draw buffers for framebuffer");
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    util::msg::fatal("framebuffer is not complete");
  }
}

}  // namespace crystal::opengl
