#include <chrono>
#include <cstdint>
#include <thread>
#include <type_traits>

#include "SDL.h"
#include "examples/triangle/triangle_view.hpp"
#include "util/msg/msg.hpp"

namespace {

template <typename Ctx>
SDL_Window* create_window(const char* title, uint32_t width, uint32_t height) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    util::msg::fatal("initializing SDL: ", SDL_GetError());
  }

  uint32_t flags = SDL_WINDOW_HIDDEN;
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

}  // namespace

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    util::msg::fatal("initializing SDL: ", SDL_GetError());
  }

  float angle = 0.0f;

#if 1
  auto window = create_window<crystal::opengl::Context>("triangle (opengl)", 1280, 720);
  auto ctx    = crystal::opengl::Context(crystal::opengl::Context::Desc{}, window);
  auto view   = TriangleView<crystal::opengl::Context>::create(ctx, angle);
#else
  auto window = create_window<crystal::vulkan::Context>("triangle (vulkan)", 1280, 720);
  auto ctx    = crystal::vulkan::Context(crystal::vulkan::Context::Desc{
                                          /* .application_name         = */ "triangle",
                                          /* .max_descriptor_set_count = */ 16,
                                          /* .buffer_descriptor_count  = */ 16,
                                          /* .texture_descriptor_count = */ 16,
                                      },
                                      window);
  auto view   = TriangleView<crystal::vulkan::Context>::create(ctx, angle);
#endif

  // Render the first frame before showing the window so that we don't flash an unrendered window.
  view->frame(angle);
  SDL_ShowWindow(window);

  using namespace std::chrono_literals;
  static constexpr auto MICROSECS_PER_FRAME = 16666us;

  using clock                = std::chrono::high_resolution_clock;
  auto prev_frame_start_time = clock::now();

  bool running = true;
  while (running) {
    const auto curr_frame_start_time = prev_frame_start_time + MICROSECS_PER_FRAME;

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_KEYUP:
          if (sdl_event.key.repeat != 0) {
            break;
          }
          break;
      }
    }

    angle += glm::radians(1.0f);
    view->frame(angle);

    // Sleep for 1/60 second (one frame).
    std::this_thread::sleep_until(curr_frame_start_time + MICROSECS_PER_FRAME);
  }

  view = nullptr;
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
