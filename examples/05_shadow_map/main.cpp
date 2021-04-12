#include "examples/05_shadow_map/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"

int main(int argc, char* argv[]) {
#include "examples/base/default_engine.inl"

  engine::Controller ctrl(engine::create_engine<Window, Ctx>("shadow map"),
                          std::make_unique<examples::shadow_map::Scene>());
  ctrl.run();
  return 0;
}
