#include "crystal/metal/context.hpp"

#include "SDL.h"
#include "SDL_syswm.h"
#include "util/msg/msg.hpp"

namespace crystal::metal {

#ifdef CRYSTAL_USE_SDL2

Context::Context(const Context::Desc& desc) : window_(desc.window) {
  const MTLPixelFormat PIXEL_FORMAT = MTLPixelFormatBGRA8Unorm_sRGB;

  SDL_SysWMinfo window_info = {};
  if (!SDL_GetWindowWMInfo(desc.window, &window_info)) {
    util::msg::fatal("getting raw window from SDL2");
  }

  device_        = MTLCreateSystemDefaultDevice();
  command_queue_ = [device_ newCommandQueue];

  metal_layer_ = [CAMetalLayer layer];
  [metal_layer_ setDevice:device_];
  [metal_layer_ setPixelFormat:PIXEL_FORMAT];
  [metal_layer_ setFramebufferOnly:YES];
  [metal_layer_ setPresentsWithTransaction:NO];

  switch (window_info.subsystem) {
    case SDL_SYSWM_COCOA: {
      NSView* view = [window_info.info.cocoa.window contentView];
      [view setWantsLayer:YES];
      [view setLayer:metal_layer_];
      break;
    }

    default:
      util::msg::fatal("unknown window subsystem");
  }

  int width  = 0;
  int height = 0;
  SDL_GetWindowSize(window_, &width, &height);
  if (width < 0 || height < 0) {
    util::msg::fatal("window size is negative [", width, ", ", height, "]");
  }

  MTLTextureDescriptor* texture_desc = [[MTLTextureDescriptor alloc] init];
  texture_desc.width                 = width;
  texture_desc.height                = height;
  texture_desc.storageMode           = MTLStorageModePrivate;
  texture_desc.textureType           = MTLTextureType2D;
  texture_desc.sampleCount           = 1;
  texture_desc.pixelFormat           = MTLPixelFormatDepth32Float;
  screen_depth_texture_              = [device_ newTextureWithDescriptor:texture_desc];

  screen_render_pass_ = RenderPass(*this, PIXEL_FORMAT, screen_depth_texture_);
  change_resolution(width, height);
}

Context::~Context() {}

CommandBuffer Context::next_frame() {
  id<CAMetalDrawable> metal_drawable = [metal_layer_ nextDrawable];

  [screen_render_pass_.render_pass_desc_ colorAttachments][0].texture = metal_drawable.texture;

  const auto size             = [metal_layer_ drawableSize];
  screen_render_pass_.width_  = size.width;
  screen_render_pass_.height_ = size.height;

  return CommandBuffer(metal_drawable, [command_queue_ commandBuffer]);
}

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

void Context::change_resolution(uint32_t width, uint32_t height) {
  util::msg::debug("resolution size changed to ", width, ", ", height);

  // TODO: Change the depth texture size.

  [metal_layer_ setDrawableSize:CGSize{
                                    static_cast<CGFloat>(width),
                                    static_cast<CGFloat>(height),
                                }];
  screen_render_pass_.width_  = width;
  screen_render_pass_.height_ = height;
}

void Context::set_clear_color(RenderPass& render_pass, uint32_t attachment,
                              ClearValue clear_value) {
  if (attachment >= render_pass.color_count_) {
    util::msg::fatal("setting clear color for out of bounds attachment [", attachment, "]");
  }
  [render_pass.render_pass_desc_ colorAttachments][attachment].clearColor =
      MTLClearColorMake(clear_value.color.red, clear_value.color.green, clear_value.color.blue,
                        clear_value.color.alpha);
}

void Context::set_clear_depth(RenderPass& render_pass, ClearValue clear_value) {
  // if (!render_pass.has_depth_) {
  //   util::msg::fatal(
  //       "setting clear depth for render pass that does not contain a depth attachment");
  // }
  // render_pass.clear_depth_.clear_value = clear_value;
}

}  // namespace crystal::metal
