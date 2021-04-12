#include "examples/01_triangle/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"

int main(int argc, char* argv[]) {
#include "examples/base/default_engine.inl"

  engine::Controller ctrl(engine::create_engine<Window, Ctx>("triangle"),
                          std::make_unique<examples::triangle::Scene>());
  ctrl.run();
  return 0;
}
