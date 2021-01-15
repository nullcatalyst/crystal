#include "crystal/opengl/context.hpp"

#include "SDL.h"

namespace crystal::opengl {

#ifdef CRYSTAL_USE_SDL2

Context::Context(const Context::Desc& desc, SDL_Window* window) : window_(window) {
  context_ = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, context_);

  // gladLoadGLES2Loader(SDL_GL_GetProcAddress);
  gladLoadGLLoader(SDL_GL_GetProcAddress);

  int width  = 0;
  int height = 0;
  SDL_GetWindowSize(window_, &width, &height);
  if (width < 0 || height < 0) {
    util::msg::fatal("window size is negative [", width, ", ", height, "]");
  }

  width_  = width;
  height_ = height;
  screen_render_pass_.unsafe_call_ctor(*this);
}

Context::~Context() {
  if (buffers_.size() != 0) {
    util::msg::fatal("not all shared buffers have been released (there are still ", buffers_.size(),
                     " remaining), leaking memory");
  }

  SDL_GL_DeleteContext(context_);
}

CommandBuffer Context::next_frame() {
  SDL_GL_MakeCurrent(window_, context_);
  return CommandBuffer(window_, width_, height_);
}

#endif  // CRYSTAL_USE_SDL2

void Context::change_resolution(uint32_t width, uint32_t height) {
  width_  = width;
  height_ = height;
  util::msg::info("resolution size changed to ", width, ", ", height);
  glViewport(0, 0, width_, height_);
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
