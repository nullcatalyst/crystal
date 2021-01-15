#include "crystal/opengl/render_pass.hpp"

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
  if (ctx_ != nullptr) {
    glDeleteFramebuffers(1, &framebuffer_);
  }

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

RenderPass::~RenderPass() {
  if (ctx_ == nullptr) {
    return;
  }

  glDeleteFramebuffers(1, &framebuffer_);
}

RenderPass::RenderPass(Context& ctx)
    : ctx_(&ctx), framebuffer_(0), width_(ctx.width_), height_(ctx.height_) {}

// TODO: Implement creation of opengl framebuffers.
// RenderPass::RenderPass(Context& ctx, const RenderPassDesc& desc)
//     : ctx_(&ctx), framebuffer_(0), width_(ctx.width_), height_(ctx.height_) {}

}  // namespace crystal::opengl
