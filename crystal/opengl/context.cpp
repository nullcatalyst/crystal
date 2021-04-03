#include "crystal/opengl/context.hpp"

#include "SDL.h"

namespace crystal::opengl {

#ifdef CRYSTAL_USE_SDL2

Context::Context(const Context::Desc& desc) : window_(desc.window), screen_render_pass_(*this) {
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
  glEnable(GL_FRAMEBUFFER_SRGB);
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

  SDL_GL_DeleteContext(context_);
}

void Context::set_active() { SDL_GL_MakeCurrent(window_, context_); }

CommandBuffer Context::next_frame() {
  SDL_GL_MakeCurrent(window_, context_);
  return CommandBuffer(window_);
}

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

Context::Context(const Context::Desc& desc) : screen_render_pass_(*this) {
  change_resolution(desc.width, desc.height);
}

Context::~Context() {
  if (buffers_.size() != 0) {
    util::msg::fatal("not all shared buffers have been released (there are still ", buffers_.size(),
                     " remaining), leaking memory");
  }
}

void Context::set_active() {}

CommandBuffer Context::next_frame() { return CommandBuffer(); }

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)

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
