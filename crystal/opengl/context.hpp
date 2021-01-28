#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string_view>

#include "crystal/opengl/command_buffer.hpp"
#include "crystal/opengl/gl.hpp"
#include "crystal/opengl/index_buffer.hpp"
#include "crystal/opengl/library.hpp"
#include "crystal/opengl/mesh.hpp"
#include "crystal/opengl/pipeline.hpp"
#include "crystal/opengl/render_pass.hpp"
#include "crystal/opengl/uniform_buffer.hpp"
#include "crystal/opengl/vertex_buffer.hpp"
#include "util/memory/ref_count.hpp"

#ifdef CRYSTAL_USE_SDL2

typedef struct SDL_Window SDL_Window;
typedef void*             SDL_GLContext;

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

namespace crystal::opengl {

class CommandBuffer;
class IndexBuffer;
class Library;
class Mesh;
class Pipeline;
class RenderPass;
class Texture;
class UniformBuffer;
class VertexBuffer;

class Context {
public:
  using CommandBuffer = ::crystal::opengl::CommandBuffer;
  using IndexBuffer   = ::crystal::opengl::IndexBuffer;
  using Library       = ::crystal::opengl::Library;
  using Mesh          = ::crystal::opengl::Mesh;
  using Pipeline      = ::crystal::opengl::Pipeline;
  using RenderPass    = ::crystal::opengl::RenderPass;
  using Texture       = ::crystal::opengl::Texture;
  using UniformBuffer = ::crystal::opengl::UniformBuffer;
  using VertexBuffer  = ::crystal::opengl::VertexBuffer;

#ifdef CRYSTAL_USE_SDL2

  struct Desc {
    SDL_Window* window;
  };

private:
  SDL_Window*                                 window_;
  SDL_GLContext                               context_;
  RenderPass                                  screen_render_pass_;
  std::vector<util::memory::RefCount<GLuint>> buffers_;

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

  struct Desc {
    uint32_t width;
    uint32_t height;
  };

private:
  RenderPass                                  screen_render_pass_;
  std::vector<util::memory::RefCount<GLuint>> buffers_;

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)

public:
  Context() = delete;
  Context(const Desc& desc);

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&& other);
  Context& operator=(Context&& other);

  ~Context();

  [[nodiscard]] constexpr uint32_t    screen_width() const { return screen_render_pass_.width_; }
  [[nodiscard]] constexpr uint32_t    screen_height() const { return screen_render_pass_.height_; }
  [[nodiscard]] constexpr RenderPass& screen_render_pass() { return screen_render_pass_; }

  void change_resolution(uint32_t width, uint32_t height);

  void wait();

  CommandBuffer next_frame();

  RenderPass create_render_pass(const RenderPassDesc& desc);

  Library  create_library(const std::string_view base_path);
  Pipeline create_pipeline(Library& library, RenderPass& render_pass, const PipelineDesc& desc);

  UniformBuffer create_uniform_buffer(size_t byte_length);
  UniformBuffer create_uniform_buffer(const void* const data_ptr, const size_t byte_length);
  void          update_uniform_buffer(UniformBuffer& uniform_buffer, const void* const data_ptr,
                                      const size_t byte_length);

  VertexBuffer create_vertex_buffer(size_t byte_length);
  VertexBuffer create_vertex_buffer(const void* const data_ptr, const size_t byte_length);
  void         update_vertex_buffer(VertexBuffer& vertex_buffer, const void* const data_ptr,
                                    const size_t byte_length);

  IndexBuffer create_index_buffer(size_t byte_length);
  IndexBuffer create_index_buffer(const uint16_t* const data_ptr, const size_t byte_length);
  void        update_index_buffer(IndexBuffer& vertex_buffer, const uint16_t* const data_ptr,
                                  const size_t byte_length);

  Mesh create_mesh(const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings);
  Mesh create_mesh(const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
                   const IndexBuffer& index_buffer);

private:
  friend CommandBuffer;
  friend IndexBuffer;
  friend Library;
  friend Mesh;
  friend Pipeline;
  friend RenderPass;
  friend Texture;
  friend UniformBuffer;
  friend VertexBuffer;

  void add_buffer_(GLuint buffer) noexcept;
  void retain_buffer_(GLuint buffer) noexcept;
  void release_buffer_(GLuint buffer) noexcept;
};

inline void Context::wait() {}

inline Library Context::create_library(const std::string_view spv_path) {
  return Library(std::string(spv_path));
}

inline RenderPass Context::create_render_pass(const RenderPassDesc& desc) {
  return RenderPass(*this, desc);
}

inline Pipeline Context::create_pipeline(Library& library, RenderPass& render_pass,
                                         const PipelineDesc& desc) {
  return Pipeline(*this, library, desc);
}

inline UniformBuffer Context::create_uniform_buffer(size_t byte_length) {
  return UniformBuffer(*this, byte_length);
}

inline UniformBuffer Context::create_uniform_buffer(const void* const data_ptr,
                                                    const size_t      byte_length) {
  return UniformBuffer(*this, data_ptr, byte_length);
}

inline void Context::update_uniform_buffer(UniformBuffer&    uniform_buffer,
                                           const void* const data_ptr, const size_t byte_length) {
  uniform_buffer.update(data_ptr, byte_length);
}

inline VertexBuffer Context::create_vertex_buffer(size_t byte_length) {
  return VertexBuffer(*this, byte_length);
}

inline VertexBuffer Context::create_vertex_buffer(const void* const data_ptr,
                                                  const size_t      byte_length) {
  return VertexBuffer(*this, data_ptr, byte_length);
}

inline void Context::update_vertex_buffer(VertexBuffer& vertex_buffer, const void* const data_ptr,
                                          const size_t byte_length) {
  vertex_buffer.update(data_ptr, byte_length);
}

inline IndexBuffer Context::create_index_buffer(size_t byte_length) {
  return IndexBuffer(*this, byte_length);
}

inline IndexBuffer Context::create_index_buffer(const uint16_t* const data_ptr,
                                                const size_t          byte_length) {
  return IndexBuffer(*this, data_ptr, byte_length);
}

inline void Context::update_index_buffer(IndexBuffer& index_buffer, const uint16_t* const data_ptr,
                                         const size_t byte_length) {
  index_buffer.update(data_ptr, byte_length);
}

inline Mesh Context::create_mesh(
    const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings) {
  return Mesh(*this, bindings);
}

inline Mesh Context::create_mesh(
    const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
    const IndexBuffer&                                                     index_buffer) {
  return Mesh(*this, bindings, index_buffer);
}

}  // namespace crystal::opengl
