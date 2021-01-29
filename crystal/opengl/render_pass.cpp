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

RenderPass::RenderPass(Context& ctx)
    : ctx_(&ctx) {}

// TODO: Implement creation of opengl framebuffers.
// RenderPass::RenderPass(Context& ctx, const RenderPassDesc& desc)
//     : ctx_(&ctx), framebuffer_(0), width_(ctx.width_), height_(ctx.height_) {}

}  // namespace crystal::opengl
