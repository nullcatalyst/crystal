#include "examples/base/controller.hpp"

#include <chrono>
#include <thread>

#include "examples/base/engine.hpp"
#include "examples/base/scene.hpp"

namespace engine {

void Controller::exit() {
  scene_  = nullptr;
  engine_ = nullptr;
}

void Controller::run() {
  using namespace std::chrono_literals;
  static constexpr auto MICROSECS_PER_FRAME = 16666us;

  tick_ = 1;

  engine_->init_graphics(*this, *scene_);

  using clock                = std::chrono::high_resolution_clock;
  auto prev_frame_start_time = clock::now();

  for (;;) {
    const auto curr_frame_start_time = prev_frame_start_time + MICROSECS_PER_FRAME;

    {  // Update the window.
      if (engine_ == nullptr) {
        return;
      }

      engine_->update(*this);
    }

    {  // Update the scene.
      if (scene_ == nullptr) {
        return;
      }

      scene_->frame(*this);
      scene_->tick(*this);
    }

    ++tick_;

    const auto now = clock::now();
    if (now < curr_frame_start_time + MICROSECS_PER_FRAME) {
      // Sleep for 1/60 second (one frame).
      std::this_thread::sleep_until(curr_frame_start_time + MICROSECS_PER_FRAME);
    } else {
      // This frame took too long.
      prev_frame_start_time = now - MICROSECS_PER_FRAME;
    }

    // Sleep for 1/60 second (one frame).
    std::this_thread::sleep_until(curr_frame_start_time + MICROSECS_PER_FRAME);
  }
}

}  // namespace engine
