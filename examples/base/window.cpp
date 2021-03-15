#include "examples/base/window.hpp"

#include "examples/base/controller.hpp"

namespace engine {

void Window::update(Controller& ctrl) {
  SDL_Event sdl_event;
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
      case SDL_QUIT:
        ctrl.exit();
        return;

      case SDL_KEYUP:
        if (sdl_event.key.repeat != 0) {
          break;
        }
        //   ctrl.scene().key_up(ctrl, engine::KeyEvent{
        //                                 engine::KeyCode(sdl_event.key.keysym.scancode),
        //                             });
        break;
    }
  }
}

}  // namespace engine
