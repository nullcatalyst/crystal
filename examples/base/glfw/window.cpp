#include "examples/base/glfw/window.hpp"

#include "examples/base/controller.hpp"

namespace {

int to_key(int glfw_key) {
  switch (glfw_key) {
    case GLFW_KEY_1:
      return 1;
    case GLFW_KEY_2:
      return 2;
    case GLFW_KEY_3:
      return 3;
    case GLFW_KEY_4:
      return 4;
    case GLFW_KEY_5:
      return 5;
    case GLFW_KEY_6:
      return 6;
    case GLFW_KEY_7:
      return 7;
    case GLFW_KEY_8:
      return 8;
    case GLFW_KEY_9:
      return 9;
    case GLFW_KEY_0:
      return 10;
    case GLFW_KEY_ESCAPE:
      return 11;
    default:
      return 0;
  }
}

}  // namespace

namespace engine::glfw {

void Window::update(Controller& ctrl) {
  ctrl_ = &ctrl;
  glfwPollEvents();
}

void Window::on_close_window(GLFWwindow* glfw_window) {
  Window*     window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  Controller& ctrl   = *window->ctrl_;

  ctrl.exit();
}

GLFWmonitor* Window::get_best_monitor(GLFWwindow* glfw_window) {
  int           monitor_count = 0;
  GLFWmonitor** monitors      = glfwGetMonitors(&monitor_count);
  if (monitors == nullptr) {
    return nullptr;
  }

  int window_x      = 0;
  int window_y      = 0;
  int window_width  = 0;
  int window_height = 0;
  glfwGetWindowPos(glfw_window, &window_x, &window_y);
  glfwGetWindowSize(glfw_window, &window_width, &window_height);

  GLFWmonitor* best_monitor = nullptr;
  int          best_area    = 0;

  for (int i = 0; i < monitor_count; ++i) {
    GLFWmonitor* monitor = monitors[i];

    int monitor_x = 0;
    int monitor_y = 0;
    glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode == nullptr) {
      continue;
    }

    int area_min_x = std::max(window_x, monitor_x);
    int area_min_y = std::max(window_y, monitor_y);

    int area_max_x = std::min(window_x + window_width, monitor_x + mode->width);
    int area_max_y = std::min(window_y + window_height, monitor_y + mode->height);

    int area = (area_max_x - area_min_x) * (area_max_y - area_min_y);

    if (area > best_area) {
      best_area    = area;
      best_monitor = monitor;
    }
  }

  return best_monitor;
}

void Window::center_window_on_monitor(GLFWwindow* glfw_window, GLFWmonitor* glfw_monitor) {
  if (glfw_monitor == nullptr) {
    return;
  }

  const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitor);
  if (mode == nullptr) {
    return;
  }

  int monitor_x = 0;
  int monitor_y = 0;
  glfwGetMonitorPos(glfw_monitor, &monitor_x, &monitor_y);

  int window_width  = 0;
  int window_height = 0;
  glfwGetWindowSize(glfw_window, &window_width, &window_height);

  glfwSetWindowPos(glfw_window, monitor_x + (mode->width - window_width) / 2,
                   monitor_y + (mode->height - window_height) / 2);
}

void Window::on_key(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
  Window*     window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
  Controller& ctrl   = *window->ctrl_;

  switch (action) {
    case GLFW_PRESS:
      ctrl.scene().key_down(ctrl, to_key(key));
      break;

    case GLFW_RELEASE:
      ctrl.scene().key_up(ctrl, to_key(key));
      break;
  }
}

}  // namespace engine::glfw
