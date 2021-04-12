#include "examples/04_render_to_texture/scene.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/engine.hpp"

int main(int argc, char* argv[]) {
#include "examples/base/default_engine.inl"

  engine::Controller ctrl(engine::create_engine<Window, Ctx>("render to texture"),
                          std::make_unique<examples::render_to_texture::Scene>());
  ctrl.run();
  return 0;
}
