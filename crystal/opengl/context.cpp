#include "crystal/opengl/context.hpp"

#if CRYSTAL_USE_SDL2
#include "SDL.h"
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
#include "GLFW/glfw3.h"
#endif  // ^^^ CRYSTAL_USE_GLFW

namespace crystal::opengl {

Context::Context(const Context::Desc& desc) : screen_render_pass_(*this) {
  int width  = desc.width;
  int height = desc.height;

#if CRYSTAL_USE_SDL2
  if (desc.sdl_window != nullptr) {
    sdl_window_  = desc.sdl_window;
    sdl_context_ = SDL_GL_CreateContext(sdl_window_);
    SDL_GL_MakeCurrent(sdl_window_, sdl_context_);

    // gladLoadGLES2Loader(SDL_GL_GetProcAddress);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GetWindowSize(sdl_window_, &width, &height);
    goto init_resize;
  }
#endif  // CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  if (desc.glfw_window != nullptr) {
    glfw_window_ = desc.glfw_window;
    glfwMakeContextCurrent(glfw_window_);

    // gladLoadGLES2Loader(glfwGetProcAddress);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    glfwGetWindowSize(glfw_window_, &width, &height);

    goto init_resize;
  }
#endif  // CRYSTAL_USE_GLFW

init_resize:
  if (width <= 0 || height <= 0) {
    util::msg::fatal("window size is less than or equal to zero [", width, ", ", height, "]");
  }
  change_resolution(width, height);
  GL_ASSERT(glEnable(GL_FRAMEBUFFER_SRGB), "enabling sRGB");
  // glEnable(GL_MULTISAMPLE);
}

Context::~Context() {
  screen_render_pass_.destroy();

  if (buffers_.size() != 0) {
    util::msg::fatal("not all shared buffers have been released (there are still ", buffers_.size(),
                     " remaining), leaking memory");
  }

  if (textures_.size() != 0) {
    util::msg::fatal("not all shared textures have been released (there are still ",
                     textures_.size(), " remaining), leaking memory");
  }

#if CRYSTAL_USE_SDL2
  if (sdl_window_ != nullptr) {
    SDL_GL_DeleteContext(sdl_context_);
  }
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  if (glfw_window_ != nullptr) {
    // There is no separate context to clean up.
  }
#endif  // ^^^ CRYSTAL_USE_GLFW
}

void Context::set_active() {
#if CRYSTAL_USE_SDL2
  if (sdl_window_ != nullptr) {
    SDL_GL_MakeCurrent(sdl_window_, sdl_context_);
    return;
  }
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  if (glfw_window_ != nullptr) {
    glfwMakeContextCurrent(glfw_window_);
    return;
  }
#endif  // ^^^ CRYSTAL_USE_GLFW
}

CommandBuffer Context::next_frame() {
#if CRYSTAL_USE_SDL2
  if (sdl_window_ != nullptr) {
    SDL_GL_MakeCurrent(sdl_window_, sdl_context_);
    return CommandBuffer(sdl_window_);
  }
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  if (glfw_window_ != nullptr) {
    glfwMakeContextCurrent(glfw_window_);
    return CommandBuffer(glfw_window_);
  }
#endif  // ^^^ CRYSTAL_USE_GLFW

  return CommandBuffer();
}

void Context::change_resolution(uint32_t width, uint32_t height) {
  util::msg::debug("resolution size changed to ", width, ", ", height);
  screen_render_pass_.width_  = width;
  screen_render_pass_.height_ = height;
}

void Context::set_clear_color(RenderPass& render_pass, uint32_t attachment,
                              ClearValue clear_value) {
  if (attachment >= render_pass.attachment_count_) {
    util::msg::fatal("setting clear color for out of bounds attachment [", attachment, "]");
  }
  render_pass.clear_colors_[attachment].clear_value = clear_value;
}

void Context::set_clear_depth(RenderPass& render_pass, ClearValue clear_value) {
  if (!render_pass.has_depth_) {
    util::msg::fatal(
        "setting clear depth for render pass that does not contain a depth attachment");
  }
  render_pass.clear_depth_.clear_value = clear_value;
}

void Context::add_buffer_(GLuint buffer) noexcept { buffers_.emplace_back(buffer); }

void Context::retain_buffer_(GLuint buffer) noexcept {
  auto it = std::find_if(buffers_.begin(), buffers_.end(),
                         [buffer](const auto& value) { return value.id == buffer; });
  if (it == buffers_.end()) {
    util::msg::fatal("retaining buffer that does not exist");
  }

  ++it->ref_count;
}

void Context::release_buffer_(GLuint buffer) noexcept {
  auto it = std::find_if(buffers_.begin(), buffers_.end(),
                         [buffer](const auto& value) { return value.id == buffer; });
  if (it == buffers_.end()) {
    util::msg::fatal("releasing buffer that does not exist");
  }

  if (--it->ref_count == 0) {
    GL_ASSERT(glDeleteBuffers(1, &buffer), "deleting buffer");
    buffers_.erase(it);
  }
}

void Context::add_texture_(GLuint texture) noexcept { textures_.emplace_back(texture); }

void Context::retain_texture_(GLuint texture) noexcept {
  auto it = std::find_if(textures_.begin(), textures_.end(),
                         [texture](const auto& value) { return value.id == texture; });
  if (it == textures_.end()) {
    util::msg::fatal("retaining texture that does not exist");
  }

  ++it->ref_count;
}

void Context::release_texture_(GLuint texture) noexcept {
  auto it = std::find_if(textures_.begin(), textures_.end(),
                         [texture](const auto& value) { return value.id == texture; });
  if (it == textures_.end()) {
    util::msg::fatal("releasing texture that does not exist");
  }

  if (--it->ref_count == 0) {
    GL_ASSERT(glDeleteTextures(1, &texture), "deleting texture");
    textures_.erase(it);
  }
}

}  // namespace crystal::opengl
