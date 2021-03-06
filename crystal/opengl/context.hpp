#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string_view>
#include <vector>

#include "crystal/opengl/command_buffer.hpp"
#include "crystal/opengl/gl.hpp"
#include "crystal/opengl/index_buffer.hpp"
#include "crystal/opengl/library.hpp"
#include "crystal/opengl/mesh.hpp"
#include "crystal/opengl/pipeline.hpp"
#include "crystal/opengl/render_pass.hpp"
#include "crystal/opengl/texture.hpp"
#include "crystal/opengl/uniform_buffer.hpp"
#include "crystal/opengl/vertex_buffer.hpp"
#include "util/memory/ref_count.hpp"

#if CRYSTAL_USE_SDL2
typedef struct SDL_Window SDL_Window;
typedef void*             SDL_GLContext;
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
typedef struct GLFWwindow GLFWwindow;
#endif  // ^^^ CRYSTAL_USE_SDL2

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

  struct Desc {
#if CRYSTAL_USE_SDL2
    SDL_Window* sdl_window = nullptr;
#endif  // ^^^ CRYSTAL_USE_SDL2
#if CRYSTAL_USE_GLFW
    GLFWwindow* glfw_window = nullptr;
#endif  // ^^^ CRYSTAL_USE_GLFW
    uint32_t width  = 0;
    uint32_t height = 0;
  };

private:
  struct RefCountedBuffer {
    uint32_t ref_count;
    GLuint   id;

    constexpr RefCountedBuffer(GLuint id) : ref_count(1), id(id) {}
  };

  struct RefCountedTexture {
    uint32_t ref_count;
    GLuint   id;

    constexpr RefCountedTexture(GLuint id) : ref_count(1), id(id) {}
  };

#if CRYSTAL_USE_SDL2
  SDL_Window*   sdl_window_  = nullptr;
  SDL_GLContext sdl_context_ = nullptr;
#endif  // ^^^ CRYSTAL_USE_SDL2

#if CRYSTAL_USE_GLFW
  GLFWwindow* glfw_window_ = nullptr;
#endif  // ^^^ CRYSTAL_USE_GLFW

  RenderPass                     screen_render_pass_;
  std::vector<RefCountedBuffer>  buffers_;
  std::vector<RefCountedTexture> textures_;

public:
  Context() = delete;
  Context(const Desc& desc);

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&& other);
  Context& operator=(Context&& other);

  ~Context();

#include "crystal/common/context_methods.inl"

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

  void add_texture_(GLuint texture) noexcept;
  void retain_texture_(GLuint texture) noexcept;
  void release_texture_(GLuint texture) noexcept;
};

inline constexpr uint32_t    Context::screen_width() const { return screen_render_pass_.width(); }
inline constexpr uint32_t    Context::screen_height() const { return screen_render_pass_.height(); }
inline constexpr RenderPass& Context::screen_render_pass() { return screen_render_pass_; }

inline void Context::wait() {}

inline Texture Context::create_texture(const TextureDesc& desc) { return Texture(*this, desc); }

inline RenderPass Context::create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures) {
  return RenderPass(*this, color_textures);
}

inline RenderPass Context::create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
    const std::tuple<const Texture&, AttachmentDesc>                        depth_texture) {
  return RenderPass(*this, color_textures, depth_texture);
}

inline Library Context::create_library(const std::string_view library_file_path) {
  return Library(std::string(library_file_path));
}

inline Pipeline Context::create_pipeline(Library& library, RenderPass& render_pass,
                                         const PipelineDesc& desc) {
  return Pipeline(*this, library, desc);
}

inline UniformBuffer Context::create_uniform_buffer(const size_t byte_length) {
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

inline VertexBuffer Context::create_vertex_buffer(const size_t byte_length) {
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

inline IndexBuffer Context::create_index_buffer(const size_t byte_length) {
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
