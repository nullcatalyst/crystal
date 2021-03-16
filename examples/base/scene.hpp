#pragma once

#include "crystal/crystal.hpp"
#include "util/msg/msg.hpp"

namespace engine {

class Controller;

class Scene {
public:
  virtual ~Scene() = default;

  virtual void tick(Controller& ctrl)  = 0;
  virtual void frame(Controller& ctrl) = 0;

  template <typename Context>
  void         init_graphics(Controller& ctrl, Context& ctx);
  virtual void destroy_graphics() = 0;

protected:
  virtual void init_graphics_opengl(Controller& ctrl, crystal::opengl::Context& ctx) {
    util::msg::fatal("unsupported graphics context type: OpenGL");
  }

  virtual void init_graphics_vulkan(Controller& ctrl, crystal::vulkan::Context& ctx) {
    util::msg::fatal("unsupported graphics context type: Vulkan");
  }

  virtual void init_graphics_metal(Controller& ctrl, crystal::metal::Context& ctx) {
    util::msg::fatal("unsupported graphics context type: Metal");
  }
};

#if CRYSTAL_USE_OPENGL

template <>
inline void Scene::init_graphics<crystal::opengl::Context>(engine::Controller&       ctrl,
                                                           crystal::opengl::Context& ctx) {
  init_graphics_opengl(ctrl, ctx);
}

#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN

template <>
inline void Scene::init_graphics<crystal::vulkan::Context>(engine::Controller&       ctrl,
                                                           crystal::vulkan::Context& ctx) {
  init_graphics_vulkan(ctrl, ctx);
}

#endif  // ^^^ CRYSTAL_USE_VULKAN

#if CRYSTAL_USE_METAL

template <>
inline void Scene::init_graphics<crystal::metal::Context>(engine::Controller&      ctrl,
                                                          crystal::metal::Context& ctx) {
  init_graphics_metal(ctrl, ctx);
}

#endif  // ^^^ CRYSTAL_USE_METAL

}  // namespace engine
