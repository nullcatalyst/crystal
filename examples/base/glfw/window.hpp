#pragma once

#include "GLFW/glfw3.h"
#include "crystal/crystal.hpp"
#include "util/msg/msg.hpp"

namespace engine {

class Controller;

}  // namespace engine

namespace engine::glfw {

class Window {
  GLFWwindow* glfw_;
  Controller* ctrl_;

public:
  template <typename Ctx>
  static Window create_window(const char* title, uint32_t width, uint32_t height) {
    if (!glfwInit()) {
      util::msg::fatal("initializing GLFW");
    }

    glfwDefaultWindowHints();

    if constexpr (std::is_same<Ctx, crystal::opengl::Context>::value) {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
      glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
      glfwWindowHint(GLFW_SAMPLES, 4);
    }
    if constexpr (std::is_same<Ctx, crystal::vulkan::Context>::value) {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
    if constexpr (std::is_same<Ctx, crystal::metal::Context>::value) {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwSwapInterval(0);

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
      util::msg::fatal("opening GLFW window");
    }

    center_window_on_monitor(window, get_best_monitor(window));

    return window;
  }

  constexpr Window() : glfw_(nullptr), ctrl_(nullptr) {}
  Window(GLFWwindow* glfw_window) : glfw_(glfw_window), ctrl_(nullptr) {
    glfwSetWindowUserPointer(glfw_, this);

    glfwSetWindowCloseCallback(glfw_, on_close_window);
    glfwSetKeyCallback(glfw_, on_key);
  }

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  Window(Window&& other) : glfw_(other.glfw_) {
    other.glfw_ = nullptr;
    glfwSetWindowUserPointer(glfw_, this);
  }
  Window& operator=(Window&& other) {
    glfw_       = other.glfw_;
    other.glfw_ = nullptr;
    glfwSetWindowUserPointer(glfw_, this);
    return *this;
  }

  ~Window() {
    if (glfw_ != nullptr) {
      glfwDestroyWindow(glfw_);
    }
  }

  [[nodiscard]] constexpr operator GLFWwindow*() const { return glfw_; }

  void show() { glfwShowWindow(glfw_); }
  void update(Controller& ctrl);

private:
  static GLFWmonitor* get_best_monitor(GLFWwindow* glfw_monitor);
  static void         center_window_on_monitor(GLFWwindow* glfw_window, GLFWmonitor* glfw_monitor);

  static void on_close_window(GLFWwindow* glfw_window);
  static void on_key(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
};

}  // namespace engine::glfw
