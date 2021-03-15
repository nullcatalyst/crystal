#pragma once

#include <memory>

#include "examples/base/engine.hpp"
#include "examples/base/scene.hpp"

namespace engine {

class Engine;
class Scene;

class Controller {
  std::unique_ptr<engine::Engine> engine_;
  std::unique_ptr<engine::Scene>  scene_;
  uint32_t                        tick_;

public:
  Controller(std::unique_ptr<engine::Engine>&& engine, std::unique_ptr<engine::Scene>&& scene)
      : engine_(std::move(engine)), scene_(std::move(scene)), tick_(0) {}

  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

  Controller(Controller&&) = default;
  Controller& operator=(Controller&&) = default;

  ~Controller() = default;

  [[nodiscard]] Engine&            engine() const { return *engine_; }
  [[nodiscard]] Scene&             scene() const { return *scene_; }
  [[nodiscard]] constexpr uint32_t tick() const { return tick_; }

  template <typename Engine, typename... EngineArgs>
  void change_engine(EngineArgs&&... engine_args) {
    scene_->destroy_graphics();
    engine_ = nullptr;
    engine_ = std::make_unique<Engine>(std::forward<EngineArgs>(engine_args)...);
    engine_->init_graphics(*this, *scene_);
  }

  template <typename Scene, typename... SceneArgs>
  void change_scene(SceneArgs&&... scene_args) {
    scene_ = nullptr;
    scene_ = std::make_unique<Engine>(std::forward<SceneArgs>(scene_args)...);
    engine_->init_graphics(*this, *scene_);
  }

  void exit();
  void run();
};

}  // namespace engine
