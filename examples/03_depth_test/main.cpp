#include "examples/03_depth_test/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"

int main(int argc, char* argv[]) {
#include "examples/base/default_engine.inl"

  engine::Controller ctrl(engine::create_engine<Window, Ctx>("depth test"),
                          std::make_unique<examples::depth_test::Scene>());
  ctrl.run();
  return 0;
}
