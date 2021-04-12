#include "examples/02_instances/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"

int main(int argc, char* argv[]) {
#include "examples/base/default_engine.inl"

  engine::Controller ctrl(engine::create_engine<Window, Ctx>("instances"),
                          std::make_unique<examples::instances::Scene>());
  ctrl.run();
  return 0;
}
