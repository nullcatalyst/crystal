#include "examples/02_instances/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"

int main(int argc, char* argv[]) {
#if CRYSTAL_USE_SDL2
  using Window = engine::sdl::Window;
#elif CRYSTAL_USE_GLFW  // ^^^ CRYSTAL_USE_SDL2 / CRYSTAL_USE_GLFW vvv
  using Window = engine::glfw::Window;
#else
#error No windowing engine chosen.
#endif  // ^^^ !CRYSTAL_USE_SDL2 && !CRYSTAL_USE_GLFW

#if CRYSTAL_USE_OPENGL
  using Ctx = crystal::opengl::Context;
#elif CRYSTAL_USE_VULKAN  // ^^^ CRYSTAL_USE_OPENGL / CRYSTAL_USE_VULKAN vvv
  using Ctx = crystal::vulkan::Context;
#elif CRYSTAL_USE_METAL   // ^^^ CRYSTAL_USE_VULKAN / CRYSTAL_USE_METAL vvv
  using Ctx = crystal::metal::Context;
#else
#error No crystal backend chosen.
#endif  // ^^^ !CRYSTAL_USE_OPENGL && !CRYSTAL_USE_VULKAN && !CRYSTAL_USE_METAL

  engine::Controller ctrl(engine::create_engine<Window, Ctx>("instances"),
                          std::make_unique<examples::instances::Scene>());
  ctrl.run();
  return 0;
}
