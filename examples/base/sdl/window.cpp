#include "examples/base/sdl/window.hpp"

#include "examples/base/controller.hpp"

namespace {

int to_key(int sdl_key) {
  switch (sdl_key) {
    case SDLK_1:
      return 1;
    case SDLK_2:
      return 2;
    case SDLK_3:
      return 3;
    case SDLK_4:
      return 4;
    case SDLK_5:
      return 5;
    case SDLK_6:
      return 6;
    case SDLK_7:
      return 7;
    case SDLK_8:
      return 8;
    case SDLK_9:
      return 9;
    case SDLK_0:
      return 10;
    case SDLK_ESCAPE:
      return 11;
    default:
      return 0;
  }
}

}  // namespace

namespace engine::sdl {

void Window::update(Controller& ctrl) {
  SDL_Event sdl_event;
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
      case SDL_QUIT:
        ctrl.exit();
        return;

      case SDL_KEYDOWN:
        if (sdl_event.key.repeat != 0) {
          break;
        }

        ctrl.scene().key_down(ctrl, to_key(sdl_event.key.keysym.sym));
        break;

      case SDL_KEYUP:
        if (sdl_event.key.repeat != 0) {
          break;
        }
        ctrl.scene().key_up(ctrl, to_key(sdl_event.key.keysym.sym));
        break;
    }
  }
}

}  // namespace engine::sdl
