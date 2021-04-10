#pragma once

#include <memory>  // unique_ptr
#include <string>
#include <string_view>

#include "crystal/crystal.hpp"
#include "examples/base/scene.hpp"

#if CRYSTAL_USE_SDL2
#include "examples/base/sdl/window.hpp"
#endif  // CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
#include "examples/base/glfw/window.hpp"
#endif  // CRYSTAL_USE_GLFW

namespace engine {

class Controller;
class Scene;

class Engine {
public:
  virtual ~Engine() = default;

  virtual void set_active()                                  = 0;
  virtual void init_graphics(Controller& ctrl, Scene& scene) = 0;
  virtual void update(Controller& ctrl)                      = 0;
};

#if CRYSTAL_HAS_CONCEPTS
template <typename Window, crystal::concepts::Context Ctx>
#else   // ^^^ CRYSTAL_HAS_CONCEPTS / !CRYSTAL_HAS_CONCEPTS vvv
template <typename Window, typename Ctx>
#endif  // ^^^ !CRYSTAL_HAS_CONCEPTS
class EngineImpl : public Engine {
  Window window_;
  Ctx    ctx_;

public:
  EngineImpl(Window&& window, const typename Ctx::Desc& desc)
      : window_(std::move(window)), ctx_(desc) {}

  virtual ~EngineImpl() { ctx_.wait(); }

  virtual void set_active() override { ctx_.set_active(); }

  virtual void init_graphics(Controller& ctrl, Scene& scene) override {
    ctx_.set_active();
    scene.init_graphics(ctrl, ctx_);
    scene.frame(ctrl);
    window_.show();
  }

  virtual void update(Controller& ctrl) override { window_.update(ctrl); }
};

template <typename Window, typename Ctx>
std::unique_ptr<EngineImpl<Window, Ctx>> create_engine(const std::string_view title) {
  std::string orig_title(title);
  std::string cust_title(title);

  typename Ctx::Desc desc = {};

#if CRYSTAL_USE_SDL2
  if constexpr (std::is_same<Window, engine::sdl::Window>::value) {
    cust_title += " (sdl + ";
  }
#endif  // CRYSTAL_USE_SDL2
#if CRYSTAL_USE_GLFW
  if constexpr (std::is_same<Window, engine::glfw::Window>::value) {
    cust_title += " (glfw + ";
  }
#endif  // CRYSTAL_USE_GLFW

#if CRYSTAL_USE_OPENGL
  if constexpr (std::is_same<Ctx, crystal::opengl::Context>::value) {
    cust_title += "opengl)";
  }
#endif  // CRYSTAL_USE_OPENGL
#if CRYSTAL_USE_VULKAN
  if constexpr (std::is_same<Ctx, crystal::vulkan::Context>::value) {
    desc.application_name         = orig_title.c_str();
    desc.max_descriptor_set_count = 16;
    desc.buffer_descriptor_count  = 16;
    desc.texture_descriptor_count = 16;
    cust_title += "vulkan)";
  }
#endif  // CRYSTAL_USE_VULKAN
#if CRYSTAL_USE_METAL
  if constexpr (std::is_same<Ctx, crystal::metal::Context>::value) {
    cust_title += "metal)";
  }
#endif  // CRYSTAL_USE_METAL

  auto window = Window::template create<Ctx>(cust_title.c_str(), 1280, 720);
#if CRYSTAL_USE_SDL2
  if constexpr (std::is_same<Window, engine::sdl::Window>::value) {
    desc.sdl_window = window;
  }
#endif  // CRYSTAL_USE_SDL2
#if CRYSTAL_USE_GLFW
  if constexpr (std::is_same<Window, engine::glfw::Window>::value) {
    desc.glfw_window = window;
  }
#endif  // CRYSTAL_USE_GLFW
  return std::make_unique<EngineImpl<Window, Ctx>>(std::move(window), desc);
}

}  // namespace engine
