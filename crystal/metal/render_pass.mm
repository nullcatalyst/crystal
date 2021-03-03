#include "crystal/metal/render_pass.hpp"

#include "crystal/metal/command_buffer.hpp"
#include "crystal/metal/context.hpp"
#include "util/msg/msg.hpp"

namespace crystal::metal {

RenderPass::RenderPass(RenderPass&& other)
    : render_pass_desc_(other.render_pass_desc_),
      width_(other.width_),
      height_(other.height_),
      color_count_(other.color_count_),
      color_pixel_formats_(other.color_pixel_formats_),
      has_depth_(other.has_depth_),
      depth_pixel_format_(other.depth_pixel_format_) {
  other.render_pass_desc_    = nullptr;
  other.width_               = 0;
  other.height_              = 0;
  other.color_count_         = 0;
  other.color_pixel_formats_ = {};
  other.has_depth_           = false;
  other.depth_pixel_format_  = {};
}

RenderPass& RenderPass::operator=(RenderPass&& other) {
  destroy();

  render_pass_desc_    = other.render_pass_desc_;
  width_               = other.width_;
  height_              = other.height_;
  color_count_         = other.color_count_;
  color_pixel_formats_ = other.color_pixel_formats_;
  has_depth_           = other.has_depth_;
  depth_pixel_format_  = other.depth_pixel_format_;

  other.render_pass_desc_    = nullptr;
  other.width_               = 0;
  other.height_              = 0;
  other.color_count_         = 0;
  other.color_pixel_formats_ = {};
  other.has_depth_           = false;
  other.depth_pixel_format_  = {};

  return *this;
}

RenderPass::~RenderPass() { destroy(); }

void RenderPass::destroy() noexcept {
  render_pass_desc_    = nullptr;
  width_               = 0;
  height_              = 0;
  color_count_         = 0;
  color_pixel_formats_ = {};
  has_depth_           = false;
  depth_pixel_format_  = {};
}

RenderPass::RenderPass(Context& ctx, MTLPixelFormat pixel_format, OBJC(MTLTexture) depth_texture)
    : color_count_(1) {
  MTLRenderPassDescriptor* render_pass_desc        = [MTLRenderPassDescriptor renderPassDescriptor];
  render_pass_desc.colorAttachments[0].loadAction  = MTLLoadActionClear;
  render_pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
  render_pass_desc.colorAttachments[0].clearColor  = MTLClearColorMake(0.0f, 0.0f, 0.0f, 0.0f);

  render_pass_desc.depthAttachment.texture     = depth_texture;
  render_pass_desc.depthAttachment.loadAction  = MTLLoadActionClear;
  render_pass_desc.depthAttachment.storeAction = MTLStoreActionDontCare;
  render_pass_desc.depthAttachment.clearDepth  = 1.0f;

  render_pass_desc_       = render_pass_desc;
  color_pixel_formats_[0] = pixel_format;
  // depth_pixel_format_     = [depth_texture pixelFormat];
}

RenderPass::RenderPass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures)
    : color_count_(static_cast<uint32_t>(color_textures.size())), has_depth_(false) {
  if (color_textures.size() == 0) {
    util::msg::fatal("framebuffer must render to at least one texture");
  }
  if (color_textures.size() > 4) {
    util::msg::fatal("framebuffer can have at most 4 color textures");
  }

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = color_textures.begin()[0];
    width_                      = static_cast<uint32_t>([texture.texture_ width]);
    height_                     = static_cast<uint32_t>([texture.texture_ height]);
  }

  MTLRenderPassDescriptor* render_pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];

  for (int i = 0; i < color_textures.size(); ++i) {
    const auto& [texture, desc] = color_textures.begin()[i];

    color_pixel_formats_[i] = texture.pixel_format_;

    render_pass_desc.colorAttachments[i].texture = texture.texture_;
    render_pass_desc.colorAttachments[i].loadAction =
        desc.clear ? MTLLoadActionClear : MTLLoadActionDontCare;
    render_pass_desc.colorAttachments[i].storeAction = MTLStoreActionStore;
    render_pass_desc.colorAttachments[i].clearColor =
        MTLClearColorMake(desc.clear_value.color.red, desc.clear_value.color.green,
                          desc.clear_value.color.blue, desc.clear_value.color.alpha);

    const auto width  = [texture.texture_ width];
    const auto height = [texture.texture_ height];
    if (width_ != width || height_ != height) {
      util::msg::fatal("all textures in a framebuffer must be of the same size: framebuffer width=",
                       width_, " height=", height_, ", texture width=", width, " height=", height);
    }
  }

  render_pass_desc_ = render_pass_desc;
}

RenderPass::RenderPass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
    const std::tuple<const Texture&, AttachmentDesc>                        depth_texture)
    : color_count_(static_cast<uint32_t>(color_textures.size())), has_depth_(true) {
  if (color_textures.size() > 4) {
    util::msg::fatal("framebuffer can have at most 4 color textures");
  }

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = depth_texture;
    width_                      = static_cast<uint32_t>([texture.texture_ width]);
    height_                     = static_cast<uint32_t>([texture.texture_ height]);
  }

  MTLRenderPassDescriptor* render_pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];

  for (int i = 0; i < color_textures.size(); ++i) {
    const auto& [texture, desc] = color_textures.begin()[i];

    color_pixel_formats_[i] = texture.pixel_format_;

    render_pass_desc.colorAttachments[i].texture = texture.texture_;
    render_pass_desc.colorAttachments[i].loadAction =
        desc.clear ? MTLLoadActionClear : MTLLoadActionDontCare;
    render_pass_desc.colorAttachments[i].storeAction = MTLStoreActionStore;
    render_pass_desc.colorAttachments[i].clearColor =
        MTLClearColorMake(desc.clear_value.color.red, desc.clear_value.color.green,
                          desc.clear_value.color.blue, desc.clear_value.color.alpha);

    const auto width  = [texture.texture_ width];
    const auto height = [texture.texture_ height];
    if (width_ != width || height_ != height) {
      util::msg::fatal("all textures in a framebuffer must be of the same size: framebuffer width=",
                       width_, " height=", height_, ", texture width=", width, " height=", height);
    }
  }

  {  // Save the depth settings.
    const auto& [texture, desc] = depth_texture;

    depth_pixel_format_ = texture.pixel_format_;

    render_pass_desc.depthAttachment.texture = texture.texture_;
    render_pass_desc.depthAttachment.loadAction =
        desc.clear ? MTLLoadActionClear : MTLLoadActionDontCare;
    render_pass_desc.depthAttachment.storeAction = MTLStoreActionStore;
    render_pass_desc.depthAttachment.clearDepth  = desc.clear_value.depth;
  }

  render_pass_desc_ = render_pass_desc;
}

}  // namespace crystal::metal
