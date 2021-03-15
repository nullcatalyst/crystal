#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string_view>
#include <vector>

#include "crystal/metal/command_buffer.hpp"
#include "crystal/metal/index_buffer.hpp"
#include "crystal/metal/library.hpp"
#include "crystal/metal/mesh.hpp"
#include "crystal/metal/mtl.hpp"
#include "crystal/metal/pipeline.hpp"
#include "crystal/metal/render_pass.hpp"
#include "crystal/metal/texture.hpp"
#include "crystal/metal/uniform_buffer.hpp"
#include "crystal/metal/vertex_buffer.hpp"

#ifdef CRYSTAL_USE_SDL2

typedef struct SDL_Window SDL_Window;

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

namespace crystal::metal {

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
  using CommandBuffer = ::crystal::metal::CommandBuffer;
  using IndexBuffer   = ::crystal::metal::IndexBuffer;
  using Library       = ::crystal::metal::Library;
  using Mesh          = ::crystal::metal::Mesh;
  using Pipeline      = ::crystal::metal::Pipeline;
  using RenderPass    = ::crystal::metal::RenderPass;
  using Texture       = ::crystal::metal::Texture;
  using UniformBuffer = ::crystal::metal::UniformBuffer;
  using VertexBuffer  = ::crystal::metal::VertexBuffer;

#ifdef CRYSTAL_USE_SDL2

  struct Desc {
    SDL_Window* window;
  };

private:
  SDL_Window* window_                    = nullptr;
  OBJC(CAMetalLayer) metal_layer_        = nullptr;
  OBJC(MTLDevice) device_                = nullptr;
  OBJC(MTLCommandQueue) command_queue_   = nullptr;
  OBJC(MTLTexture) screen_depth_texture_ = nullptr;
  RenderPass screen_render_pass_;

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

public:
  Context() = delete;
  Context(const Desc& desc);

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&& other);
  Context& operator=(Context&& other);

  ~Context();

  [[nodiscard]] constexpr uint32_t    screen_width() const { return screen_render_pass_.width(); }
  [[nodiscard]] constexpr uint32_t    screen_height() const { return screen_render_pass_.height(); }
  [[nodiscard]] constexpr RenderPass& screen_render_pass() { return screen_render_pass_; }

  void change_resolution(uint32_t width, uint32_t height);

  void wait();

  CommandBuffer next_frame();

  Texture create_texture(const TextureDesc& desc);

  RenderPass create_render_pass(
      const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures);
  RenderPass create_render_pass(
      const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
      const std::tuple<const Texture&, AttachmentDesc>                        depth_texture);

  Library  create_library(const std::string_view base_path);
  Pipeline create_pipeline(Library& library, RenderPass& render_pass, const PipelineDesc& desc);

  UniformBuffer create_uniform_buffer(size_t byte_length);
  UniformBuffer create_uniform_buffer(const void* const data_ptr, const size_t byte_length);
  void          update_uniform_buffer(UniformBuffer& uniform_buffer, const void* const data_ptr,
                                      const size_t byte_length);

  VertexBuffer create_vertex_buffer(size_t byte_length);
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

  // Templated convenience helpers.

  template <typename T>
  UniformBuffer create_uniform_buffer(const T& value) {
    return create_uniform_buffer(&value, sizeof(T));
  }

  template <typename T>
  void update_uniform_buffer(const T& value) {
    return update_uniform_buffer(&value, sizeof(T));
  }

  template <typename T>
  VertexBuffer create_vertex_buffer(const std::initializer_list<T> list) {
    return create_vertex_buffer(list.begin(), sizeof(list.begin()[0]) * list.size());
  }

  template <typename Container>
  VertexBuffer create_vertex_buffer(const Container& container) {
    return create_vertex_buffer(container.data(), sizeof(container.data()[0]) * container.size());
  }

  template <typename T>
  void update_vertex_buffer(const std::initializer_list<T> list) {
    return update_vertex_buffer(list.begin(), sizeof(list.begin()[0]) * list.size());
  }

  template <typename Container>
  void update_vertex_buffer(const Container& container) {
    return create_vertex_buffer(container.data(), sizeof(container.data()[0]) * container.size());
  }

  template <typename Container>
  IndexBuffer create_index_buffer(const Container& container) {
    static_assert(std::is_same<decltype(container.data()[0]), uint16_t>::value,
                  "index buffer type must be [uint16_t]");
    return create_index_buffer(container.data(), sizeof(container.data()[0]) * container.size());
  }

  void update_index_buffer(const std::initializer_list<uint16_t> list) {
    return update_index_buffer(list.begin(), sizeof(list.begin()[0]) * list.size());
  }

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
};

inline void Context::wait() {}

inline Library Context::create_library(const std::string_view spv_path) {
  return Library(device_, std::string(spv_path));
}

inline Texture Context::create_texture(const TextureDesc& desc) { return Texture(device_, desc); }

inline RenderPass Context::create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures) {
  return RenderPass(color_textures);
}

inline RenderPass Context::create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
    const std::tuple<const Texture&, AttachmentDesc>                        depth_texture) {
  return RenderPass(color_textures, depth_texture);
}

inline Pipeline Context::create_pipeline(Library& library, RenderPass& render_pass,
                                         const PipelineDesc& desc) {
  return Pipeline(device_, library, render_pass, desc);
}

inline UniformBuffer Context::create_uniform_buffer(size_t byte_length) {
  return UniformBuffer(device_, byte_length);
}

inline UniformBuffer Context::create_uniform_buffer(const void* const data_ptr,
                                                    const size_t      byte_length) {
  return UniformBuffer(device_, data_ptr, byte_length);
}

inline void Context::update_uniform_buffer(UniformBuffer&    uniform_buffer,
                                           const void* const data_ptr, const size_t byte_length) {
  uniform_buffer.update(data_ptr, byte_length);
}

inline VertexBuffer Context::create_vertex_buffer(size_t byte_length) {
  return VertexBuffer(device_, byte_length);
}

inline VertexBuffer Context::create_vertex_buffer(const void* const data_ptr,
                                                  const size_t      byte_length) {
  return VertexBuffer(device_, data_ptr, byte_length);
}

inline void Context::update_vertex_buffer(VertexBuffer& vertex_buffer, const void* const data_ptr,
                                          const size_t byte_length) {
  vertex_buffer.update(data_ptr, byte_length);
}

inline IndexBuffer Context::create_index_buffer(size_t byte_length) {
  return IndexBuffer(device_, byte_length);
}

inline IndexBuffer Context::create_index_buffer(const uint16_t* const data_ptr,
                                                const size_t          byte_length) {
  return IndexBuffer(device_, data_ptr, byte_length);
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

}  // namespace crystal::metal
