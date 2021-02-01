#include "crystal/metal/context.hpp"

#include "SDL.h"
#include "SDL_syswm.h"
#include "util/msg/msg.hpp"

namespace crystal::metal {

#ifdef CRYSTAL_USE_SDL2

Context::Context(const Context::Desc& desc) : window_(desc.window) {
  const MTLPixelFormat PIXEL_FORMAT = MTLPixelFormatBGRA8Unorm;

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

  change_resolution(width, height);

  screen_render_pass_.attachment_count_ = 1;
  screen_render_pass_.pixel_formats_[0] = PIXEL_FORMAT;
}

Context::~Context() {}

CommandBuffer Context::next_frame() {
  id<CAMetalDrawable> metal_drawable = [metal_layer_ nextDrawable];

  MTLRenderPassDescriptor* render_pass_desc        = [MTLRenderPassDescriptor renderPassDescriptor];
  render_pass_desc.colorAttachments[0].texture     = metal_drawable.texture;
  render_pass_desc.colorAttachments[0].loadAction  = MTLLoadActionClear;
  render_pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
  // TODO: Add support for changing clear values.
  render_pass_desc.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);

  screen_render_pass_.render_pass_desc_ = render_pass_desc;

  return CommandBuffer(metal_drawable, [command_queue_ commandBuffer]);
}

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

void Context::change_resolution(uint32_t width, uint32_t height) {
  util::msg::info("resolution size changed to ", width, ", ", height);

  [metal_layer_ setDrawableSize:CGSize{
                                    static_cast<CGFloat>(width),
                                    static_cast<CGFloat>(height),
                                }];
  screen_render_pass_.width_  = width;
  screen_render_pass_.height_ = height;
}

}  // namespace crystal::metal
