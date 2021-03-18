#pragma once

#include <memory>  // unique_ptr
#include <string>
#include <string_view>

#include "SDL.h"
#include "crystal/crystal.hpp"
#include "examples/base/scene.hpp"
#include "examples/base/window.hpp"

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
template <crystal::concepts::Context Ctx>
#else   // ^^^ CRYSTAL_HAS_CONCEPTS / !CRYSTAL_HAS_CONCEPTS vvv
template <typename Ctx>
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

template <typename Ctx>
std::unique_ptr<EngineImpl<Ctx>> create_engine(const std::string_view title) {
  std::string orig_title(title);
  std::string cust_title(title);

  typename Ctx::Desc desc = {};

  if constexpr (std::is_same<Ctx, crystal::opengl::Context>::value) {
    cust_title += " (opengl)";
  }
  if constexpr (std::is_same<Ctx, crystal::vulkan::Context>::value) {
    desc = {
        /* .application_name         = */ orig_title.c_str(),
        /* .max_descriptor_set_count = */ 16,
        /* .buffer_descriptor_count  = */ 16,
        /* .texture_descriptor_count = */ 16,
    };
    cust_title += " (vulkan)";
  }
  if constexpr (std::is_same<Ctx, crystal::metal::Context>::value) {
    cust_title += " (metal)";
  }

  auto window = create_window<Ctx>(cust_title.c_str(), 1280, 720);
  desc.window = window;
  return std::make_unique<EngineImpl<Ctx>>(std::move(window), desc);
}

}  // namespace engine
