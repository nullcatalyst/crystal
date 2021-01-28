#include "crystal/opengl/context.hpp"

#include "SDL.h"

namespace crystal::opengl {

#ifdef CRYSTAL_USE_SDL2

Context::Context(const Context::Desc& desc) : window_(desc.window) {
  context_ = SDL_GL_CreateContext(window_);
  SDL_GL_MakeCurrent(window_, context_);

  // gladLoadGLES2Loader(SDL_GL_GetProcAddress);
  gladLoadGLLoader(SDL_GL_GetProcAddress);

  int width  = 0;
  int height = 0;
  SDL_GetWindowSize(window_, &width, &height);
  if (width < 0 || height < 0) {
    util::msg::fatal("window size is negative [", width, ", ", height, "]");
  }

  change_resolution(width, height);
}

Context::~Context() {
  screen_render_pass_.destroy();

  if (buffers_.size() != 0) {
    util::msg::fatal("not all shared buffers have been released (there are still ", buffers_.size(),
                     " remaining), leaking memory");
  }

  SDL_GL_DeleteContext(context_);
}

CommandBuffer Context::next_frame() {
  SDL_GL_MakeCurrent(window_, context_);
  return CommandBuffer(window_);
}

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

Context::Context(const Context::Desc& desc)
    : window_(desc.window), width_(desc.width), height_(desc.height) {}

Context::~Context() {
  if (buffers_.size() != 0) {
    util::msg::fatal("not all shared buffers have been released (there are still ", buffers_.size(),
                     " remaining), leaking memory");
  }
}

CommandBuffer Context::next_frame() { return CommandBuffer(width_, height_); }

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)

void Context::change_resolution(uint32_t width, uint32_t height) {
  util::msg::info("resolution size changed to ", width, ", ", height);
  screen_render_pass_.width_  = width;
  screen_render_pass_.height_ = height;
}

void Context::add_buffer_(GLuint buffer) noexcept { buffers_.emplace_back(buffer); }

void Context::retain_buffer_(GLuint buffer) noexcept {
  auto it = std::find_if(buffers_.begin(), buffers_.end(),
                         [buffer](auto& value) { return value == buffer; });
  if (it == buffers_.end()) {
    util::msg::fatal("retaining buffer that does not exist");
  }

  it->retain();
}

void Context::release_buffer_(GLuint buffer) noexcept {
  auto it = std::find_if(buffers_.begin(), buffers_.end(),
                         [buffer](auto& value) { return value == buffer; });
  if (it == buffers_.end()) {
    return;
  }

  if (it->release()) {
    GL_ASSERT(glDeleteBuffers(1, &buffer), "deleting buffer");
    buffers_.erase(it);
  }
}

}  // namespace crystal::opengl