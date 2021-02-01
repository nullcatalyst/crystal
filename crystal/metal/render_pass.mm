#include "crystal/metal/render_pass.hpp"

#include "crystal/metal/command_buffer.hpp"
#include "crystal/metal/context.hpp"

namespace crystal::metal {

RenderPass::RenderPass(RenderPass&& other) : width_(other.width_), height_(other.height_) {
  other.render_pass_desc_ = nullptr;
  other.attachment_count_ = 0;
  other.pixel_formats_    = {};
  other.width_            = 0;
  other.height_           = 0;
}

RenderPass& RenderPass::operator=(RenderPass&& other) {
  destroy();

  render_pass_desc_ = other.render_pass_desc_;
  attachment_count_ = other.attachment_count_;
  pixel_formats_    = other.pixel_formats_;
  width_            = other.width_;
  height_           = other.height_;

  other.render_pass_desc_ = nullptr;
  other.attachment_count_ = 0;
  other.pixel_formats_    = {};
  other.width_            = 0;
  other.height_           = 0;

  return *this;
}

RenderPass::~RenderPass() { destroy(); }

void RenderPass::destroy() noexcept {
  render_pass_desc_ = nullptr;
  attachment_count_ = 0;
  pixel_formats_    = {};
  width_            = 0;
  height_           = 0;
}

// RenderPass::RenderPass(Context& ctx) : ctx_(&ctx) {}

// TODO: Implement creation of opengl framebuffers.
// RenderPass::RenderPass(Context& ctx, const RenderPassDesc& desc)
//     : ctx_(&ctx), framebuffer_(0), width_(ctx.width_), height_(ctx.height_) {}

}  // namespace crystal::metal
