#include "examples/03_depth_test/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"
#include "util/msg/msg.hpp"

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

  engine::Controller ctrl(engine::create_engine<Ctx>("depth test"),
                          std::make_unique<examples::depth_test::Scene>());
  ctrl.run();
  return 0;
}
