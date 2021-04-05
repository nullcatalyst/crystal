#pragma once

#include "examples/04_render_to_texture/state.hpp"
#include "examples/04_render_to_texture/view.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/scene.hpp"
#include "glm/glm.hpp"

namespace examples::render_to_texture {

class Scene : public engine::Scene {
  State                 state_;
  std::unique_ptr<View> view_;

public:
  virtual void destroy_graphics() override { view_ = nullptr; }

  virtual void tick(engine::Controller& ctrl, double dt) override {
    state_.cube_angle += dt * glm::radians(90.0f);
    state_.quad_angle += dt * glm::radians(60.0f);
  }

  virtual void frame(engine::Controller& ctrl) override {
    if (view_ != nullptr) {
      view_->frame(state_);
    }
  }

  virtual void key_down(engine::Controller& ctrl, int scancode) override {
    util::msg::info("key down=", scancode);
    switch (scancode) {
#if CRYSTAL_USE_OPENGL
      case SDL_SCANCODE_1:
        ctrl.change_engine(engine::create_engine<crystal::opengl::Context>("render to texture"));
        break;
#endif  // ^^^ CRYSTAL_USE_OPENGL
#if CRYSTAL_USE_VULKAN
      case SDL_SCANCODE_2:
        ctrl.change_engine(engine::create_engine<crystal::vulkan::Context>("render to texture"));
        break;
#endif  // ^^^ CRYSTAL_USE_VULKAN
#if CRYSTAL_USE_METAL
      case SDL_SCANCODE_3:
        ctrl.change_engine(engine::create_engine<crystal::metal::Context>("render to texture"));
        break;
#endif  // ^^^ CRYSTAL_USE_METAL
    }
  }

protected:
#if CRYSTAL_USE_OPENGL
  virtual void init_graphics_opengl(engine::Controller&       ctrl,
                                    crystal::opengl::Context& ctx) override {
    view_ = std::make_unique<ViewImpl<crystal::opengl::Context>>(ctx, state_);
  }
#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN
  virtual void init_graphics_vulkan(engine::Controller&       ctrl,
                                    crystal::vulkan::Context& ctx) override {
    view_ = std::make_unique<ViewImpl<crystal::vulkan::Context>>(ctx, state_);
  }
#endif  // ^^^ CRYSTAL_USE_VULKAN

#if CRYSTAL_USE_METAL
  virtual void init_graphics_metal(engine::Controller&      ctrl,
                                   crystal::metal::Context& ctx) override {
    view_ = std::make_unique<ViewImpl<crystal::metal::Context>>(ctx, state_);
  }
#endif  // ^^^ CRYSTAL_USE_METAL
};

}  // namespace examples::render_to_texture
