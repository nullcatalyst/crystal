#pragma once

#include "SDL.h"
#include "crystal/crystal.hpp"
#include "util/msg/msg.hpp"

namespace engine {

class Controller;

class Window {
  SDL_Window* sdl_;

public:
  constexpr Window() : sdl_(nullptr) {}
  constexpr Window(SDL_Window* sdl_window) : sdl_(sdl_window) {}

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  Window(Window&& other) : sdl_(other.sdl_) { other.sdl_ = nullptr; }
  Window& operator=(Window&& other) {
    sdl_       = other.sdl_;
    other.sdl_ = nullptr;
  }

  ~Window() { SDL_DestroyWindow(sdl_); }

  [[nodiscard]] constexpr operator SDL_Window*() const { return sdl_; }

  void update(Controller& ctrl);
};

template <typename Ctx>
static Window create_window(const char* title, uint32_t width, uint32_t height) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    util::msg::fatal("initializing SDL: ", SDL_GetError());
  }

  uint32_t flags = 0;  // SDL_WINDOW_HIDDEN;
  if constexpr (std::is_same<Ctx, crystal::opengl::Context>::value) {
    flags |= SDL_WINDOW_OPENGL;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetSwapInterval(0);
  }
  // if constexpr (std::is_same<Ctx, crystal::opengles::Context>::value) {
  //   flags |= SDL_WINDOW_OPENGL;
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  //   SDL_GL_SetSwapInterval(0);
  // }
  if constexpr (std::is_same<Ctx, crystal::vulkan::Context>::value) {
    flags |= SDL_WINDOW_VULKAN;
  }

  SDL_Window* window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
  if (window == nullptr) {
    util::msg::fatal("opening SDL window: ", SDL_GetError());
  }

  return window;
}

}  // namespace engine
