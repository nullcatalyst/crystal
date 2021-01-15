#pragma once

#include <cstdint>

#include "crystal/opengl/gl.hpp"

#ifdef CRYSTAL_USE_SDL2

typedef struct SDL_Window SDL_Window;

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES \
  Pipeline*   pipeline_;        \
  SDL_Window* window_;
#define CRYSTAL_IMPL_CTOR                                            \
  CommandBuffer(SDL_Window* window, uint32_t width, uint32_t height) \
      : pipeline_(nullptr), window_(window) {                        \
    width_  = width;                                                 \
    height_ = height;                                                \
  }

#else

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES Pipeline* pipeline_;

#define CRYSTAL_IMPL_CTOR                                               \
  CommandBuffer(uint32_t width, uint32_t height) : pipeline_(nullptr) { \
    width_  = width;                                                    \
    height_ = height;                                                   \
  }

#endif  // CRYSTAL_USE_SDL2

#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/command_buffer.inl"
