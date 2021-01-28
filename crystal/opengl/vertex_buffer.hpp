#pragma once

#include <cstddef>

#include "crystal/opengl/gl.hpp"

namespace crystal::opengl {

class Context;
class Mesh;

class VertexBuffer {
  Context* ctx_      = nullptr;
  GLuint   buffer_   = 0;
  size_t   capacity_ = 0;

public:
  constexpr VertexBuffer() = default;

  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer& operator=(const VertexBuffer&) = delete;

  VertexBuffer(VertexBuffer&& other);
  VertexBuffer& operator=(VertexBuffer&& other);

  ~VertexBuffer();

  void destroy();

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::Mesh;

  VertexBuffer(Context& ctx, const size_t byte_length);
  VertexBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length);

  void update(const void* const data_ptr, const size_t byte_length) noexcept;
};

}  // namespace crystal::opengl
