#pragma once

#include "examples/03_depth_test/state.hpp"
#include "examples/03_depth_test/view.hpp"
#include "examples/base/controller.hpp"
#include "examples/base/scene.hpp"
#include "glm/glm.hpp"

namespace examples::depth_test {

class Scene : public engine::Scene {
  State                 state_;
  std::unique_ptr<View> view_;

public:
  virtual void destroy_graphics() override { view_ = nullptr; }

  virtual void tick(engine::Controller& ctrl, double dt) override {
    state_.yaw_angle += dt * glm::radians(45.0f);
    state_.pitch_angle += dt * glm::radians(75.0f);
    state_.orbit_angle += dt * glm::radians(30.0f);
  }

  virtual void frame(engine::Controller& ctrl) override {
    if (view_ != nullptr) {
      view_->frame(state_);
    }
  }

  virtual void key_down(engine::Controller& ctrl, int key) override {
    util::msg::info("key down=", key);
    switch (key) {
      case 11:  // Escape
        ctrl.exit();
        break;

#if CRYSTAL_USE_SDL2 && CRYSTAL_USE_OPENGL
      case 1:
        ctrl.change_engine([]() {
          return engine::create_engine<engine::sdl::Window, crystal::opengl::Context>("depth test");
        });
        break;
#endif  // ^^^ CRYSTAL_USE_OPENGL
#if CRYSTAL_USE_SDL2 && CRYSTAL_USE_VULKAN
      case 2:
        ctrl.change_engine([]() {
          return engine::create_engine<engine::sdl::Window, crystal::vulkan::Context>("depth test");
        });
        break;
#endif  // ^^^ CRYSTAL_USE_VULKAN
#if CRYSTAL_USE_SDL2 && CRYSTAL_USE_METAL
      case 3:
        ctrl.change_engine([]() {
          return engine::create_engine<engine::sdl::Window, crystal::metal::Context>("depth test");
        });
        break;
#endif  // ^^^ CRYSTAL_USE_METAL

#if CRYSTAL_USE_GLFW && CRYSTAL_USE_OPENGL
      case 5:
        ctrl.change_engine([]() {
          return engine::create_engine<engine::glfw::Window, crystal::opengl::Context>(
              "depth test");
        });
        break;
#endif  // ^^^ CRYSTAL_USE_OPENGL
#if CRYSTAL_USE_GLFW && CRYSTAL_USE_VULKAN
      case 6:
        ctrl.change_engine([]() {
          return engine::create_engine<engine::glfw::Window, crystal::vulkan::Context>(
              "depth test");
        });
        break;
#endif  // ^^^ CRYSTAL_USE_VULKAN
#if CRYSTAL_USE_GLFW && CRYSTAL_USE_METAL
      case 7:
        ctrl.change_engine([]() {
          return engine::create_engine<engine::glfw::Window, crystal::metal::Context>("depth test");
        });
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

}  // namespace examples::depth_test
