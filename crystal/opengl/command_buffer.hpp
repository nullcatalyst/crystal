#pragma once

#include <cstdint>

#include "crystal/opengl/gl.hpp"

#ifdef CRYSTAL_USE_SDL2

typedef struct SDL_Window SDL_Window;

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

namespace crystal::opengl {

class Context;
class Mesh;
class Pipeline;
class RenderPass;
class Texture;
class UniformBuffer;

class CommandBuffer {
#ifdef CRYSTAL_USE_SDL2

  SDL_Window* window_      = nullptr;
  RenderPass* render_pass_ = nullptr;
  Pipeline*   pipeline_    = nullptr;

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

  RenderPass* render_pass_ = nullptr;
  Pipeline*   pipeline_    = nullptr;

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)

public:
  CommandBuffer(const CommandBuffer&) = delete;
  CommandBuffer& operator=(const CommandBuffer&) = delete;

  CommandBuffer(CommandBuffer&&) = delete;
  CommandBuffer& operator=(CommandBuffer&&) = delete;

  ~CommandBuffer();

  void use_render_pass(RenderPass& render_pass);
  void use_pipeline(Pipeline& pipeline);
  void use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t binding);
  void use_texture(Texture& texture, uint32_t binding);

  void draw(Mesh& mesh, uint32_t vertex_or_index_count, uint32_t instance_count);

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::RenderPass;

#ifdef CRYSTAL_USE_SDL2

  constexpr CommandBuffer(SDL_Window* window) : window_(window) {}

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

  constexpr CommandBuffer() = default;

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)
};

}  // namespace crystal::opengl
