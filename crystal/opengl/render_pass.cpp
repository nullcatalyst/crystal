#include "crystal/opengl/render_pass.hpp"

#include <array>

#include "crystal/opengl/command_buffer.hpp"
#include "crystal/opengl/context.hpp"
#include "crystal/opengl/texture.hpp"

namespace crystal::opengl {

RenderPass::RenderPass(RenderPass&& other)
    : ctx_(other.ctx_),
      framebuffer_(other.framebuffer_),
      width_(other.width_),
      height_(other.height_),
      attachment_count_(other.attachment_count_),
      attachments_(std::move(other.attachments_)),
      clear_colors_(std::move(other.clear_colors_)),
      clear_depth_(other.clear_depth_) {
  other.ctx_              = nullptr;
  other.framebuffer_      = 0;
  other.width_            = 0;
  other.height_           = 0;
  other.attachment_count_ = 0;
  other.clear_depth_      = {};
}

RenderPass& RenderPass::operator=(RenderPass&& other) {
  destroy();

  ctx_              = other.ctx_;
  framebuffer_      = other.framebuffer_;
  width_            = other.width_;
  height_           = other.height_;
  attachment_count_ = other.attachment_count_;
  attachments_      = std::move(other.attachments_);
  clear_colors_     = std::move(other.clear_colors_);
  clear_depth_      = other.clear_depth_;

  other.ctx_              = nullptr;
  other.framebuffer_      = 0;
  other.width_            = 0;
  other.height_           = 0;
  other.attachment_count_ = 0;
  other.clear_depth_      = {};

  return *this;
}

RenderPass::~RenderPass() { destroy(); }

void RenderPass::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  for (uint32_t i = 0; i != attachment_count_ + (has_depth_ ? 1 : 0); ++i) {
    ctx_->release_texture_(attachments_[i]);
  }

  glDeleteFramebuffers(1, &framebuffer_);

  ctx_              = nullptr;
  framebuffer_      = 0;
  width_            = 0;
  height_           = 0;
  attachment_count_ = 0;
  attachments_      = {};
  clear_colors_     = {};
  clear_depth_      = {};
}

RenderPass::RenderPass(Context& ctx) : ctx_(&ctx) {
  attachment_count_      = 1;
  clear_colors_[0].clear = true;
  clear_colors_[0].color = {0.0f, 0.0f, 0.0f, 0.0f};
  clear_depth_.clear     = true;
  clear_depth_.depth     = 1.0f;
}

RenderPass::RenderPass(
    Context&                                                                     ctx,
    const std::initializer_list<std::tuple<const Texture&, ColorAttachmentDesc>> color_textures)
    : ctx_(&ctx), framebuffer_(0), has_depth_(false) {
  if (color_textures.size() == 0) {
    util::msg::fatal("framebuffer must render to at least one texture");
  }
  if (color_textures.size() > 4) {
    util::msg::fatal("framebuffer can have at most 4 color textures");
  }

  GL_ASSERT(glGenFramebuffers(1, &framebuffer_), "generating framebuffer");
  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_), "binding framebuffer");

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = color_textures.begin()[0];
    width_                      = texture.width_;
    height_                     = texture.height_;
    attachment_count_           = color_textures.size();
  }

  std::array<GLenum, 8> draw_buffers = {};
  for (int i = 0; i < color_textures.size(); ++i) {
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;

    const auto& [texture, desc] = color_textures.begin()[i];
    GL_ASSERT(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.texture_, 0),
              "attaching texture to framebuffer");

    if (width_ != texture.width_ || height_ != texture.height_) {
      util::msg::fatal(
          "all textures in a framebuffer must be of the same size: framebuffer width=", width_,
          " height=", height_, ", texture width=", texture.width_, " height=", texture.height_);
    }

    attachments_[i]  = texture.texture_;
    clear_colors_[i] = desc;
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
    : ctx_(&ctx), framebuffer_(0), has_depth_(true) {
  if (color_textures.size() > 4) {
    util::msg::fatal("framebuffer can have at most 4 color textures");
  }

  GL_ASSERT(glGenFramebuffers(1, &framebuffer_), "generating framebuffer");
  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_), "binding framebuffer");

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = depth_texture;
    width_                      = texture.width_;
    height_                     = texture.height_;
    attachment_count_           = color_textures.size();
  }

  std::array<GLenum, 8> draw_buffers = {};
  for (int i = 0; i < color_textures.size(); ++i) {
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;

    const auto& [texture, desc] = color_textures.begin()[i];
    GL_ASSERT(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.texture_, 0),
              "attaching texture to framebuffer");

    if (width_ != texture.width_ || height_ != texture.height_) {
      util::msg::fatal(
          "all textures in a framebuffer must be of the same size: framebuffer width=", width_,
          " height=", height_, ", texture width=", texture.width_, " height=", texture.height_);
    }

    attachments_[i]  = texture.texture_;
    clear_colors_[i] = desc;
    ctx.retain_texture_(texture.texture_);
  }

  {  // Save the depth settings.
    const auto& [texture, desc] = depth_texture;
    GL_ASSERT(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.texture_, 0),
              "attaching texture to framebuffer");

    attachments_[color_textures.size()] = texture.texture_;
    clear_depth_                        = desc;
    ctx.retain_texture_(texture.texture_);
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
