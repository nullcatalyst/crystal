#include "examples/02_instances/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"
#include "examples/base/glfw/window.hpp"

int main(int argc, char* argv[]) {
#if CRYSTAL_USE_OPENGL
  using Ctx = crystal::opengl::Context;
#elif CRYSTAL_USE_VULKAN  // ^^^ CRYSTAL_USE_OPENGL / CRYSTAL_USE_VULKAN vvv
  using Ctx = crystal::vulkan::Context;
#elif CRYSTAL_USE_METAL   // ^^^ CRYSTAL_USE_VULKAN / CRYSTAL_USE_METAL vvv
  using Ctx = crystal::metal::Context;
#else
#error No crystal backend chosen.
#endif  // ^^^ !CRYSTAL_USE_OPENGL && !CRYSTAL_USE_VULKAN && !CRYSTAL_USE_METAL

  engine::Controller ctrl(engine::create_engine<engine::glfw::Window, Ctx>("instances"),
                          std::make_unique<examples::instances::Scene>());
  ctrl.run();
  return 0;
}
