#pragma once

#include <cstdint>

#include "crystal/opengl/gl.hpp"

#if CRYSTAL_USE_SDL2
typedef struct SDL_Window SDL_Window;
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
typedef struct GLFWwindow GLFWwindow;
#endif  // ^^^ CRYSTAL_USE_GLFW

namespace crystal::opengl {

class Context;
class Mesh;
class Pipeline;
class RenderPass;
class Texture;
class UniformBuffer;

class CommandBuffer {
#if CRYSTAL_USE_SDL2
  SDL_Window* sdl_window_ = nullptr;
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  GLFWwindow* glfw_window_ = nullptr;
#endif  // ^^^ CRYSTAL_USE_GLFW

  const RenderPass* render_pass_ = nullptr;
  const Pipeline*   pipeline_    = nullptr;

public:
  CommandBuffer(const CommandBuffer&) = delete;
  CommandBuffer& operator=(const CommandBuffer&) = delete;

  CommandBuffer(CommandBuffer&&) = delete;
  CommandBuffer& operator=(CommandBuffer&&) = delete;

  ~CommandBuffer();

  void use_render_pass(const RenderPass& render_pass);
  void use_pipeline(const Pipeline& pipeline);
  void use_uniform_buffer(const UniformBuffer& uniform_buffer, uint32_t binding);
  void use_texture(const Texture& texture, uint32_t binding);

  void draw(const Mesh& mesh, uint32_t vertex_or_index_count, uint32_t instance_count);

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::RenderPass;

#if CRYSTAL_USE_SDL2
  constexpr CommandBuffer(SDL_Window* sdl_window) : sdl_window_(sdl_window) {}
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  constexpr CommandBuffer(GLFWwindow* glfw_window) : glfw_window_(glfw_window) {}
#endif  // ^^^ CRYSTAL_USE_GLFW

  constexpr CommandBuffer() = default;
};

}  // namespace crystal::opengl
