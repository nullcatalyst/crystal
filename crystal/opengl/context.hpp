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
#include "crystal/opengl/texture.hpp"
#include "crystal/opengl/uniform_buffer.hpp"
#include "crystal/opengl/vertex_buffer.hpp"
#include "util/memory/ref_count.hpp"

#ifdef CRYSTAL_USE_SDL2

typedef struct SDL_Window SDL_Window;
typedef void*             SDL_GLContext;

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES                         \
public:                                                 \
  struct Desc {};                                       \
                                                        \
private:                                                \
  SDL_Window*                                 window_;  \
  SDL_GLContext                               context_; \
  std::vector<util::memory::RefCount<GLuint>> buffers_;
#define CRYSTAL_IMPL_CTOR Context(const Desc& desc, SDL_Window* window);
#define CRYSTAL_IMPL_METHODS                   \
  void add_buffer_(GLuint buffer) noexcept;    \
  void retain_buffer_(GLuint buffer) noexcept; \
  void release_buffer_(GLuint buffer) noexcept;

#else

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES \
public:                         \
  struct Desc {};               \
                                \
private:                        \
  std::vector<util::memory::RefCount<GLuint>> buffers_;
#define CRYSTAL_IMPL_CTOR Context(const Desc& desc);
#define CRYSTAL_IMPL_METHODS                   \
  void add_buffer_(GLuint buffer) noexcept;    \
  void retain_buffer_(GLuint buffer) noexcept; \
  void release_buffer_(GLuint buffer) noexcept;

#endif  // CRYSTAL_USE_SDL2

#include "crystal/interface/context.inl"

namespace crystal::opengl {

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
