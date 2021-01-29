#pragma once

#include "crystal/opengl/gl.hpp"

namespace crystal::opengl {

class Context;
class Mesh;

class IndexBuffer {
  Context* ctx_      = nullptr;
  GLuint   buffer_   = 0;
  size_t   capacity_ = 0;

public:
  constexpr IndexBuffer() = default;

  IndexBuffer(const IndexBuffer&) = delete;
  IndexBuffer& operator=(const IndexBuffer&) = delete;

  IndexBuffer(IndexBuffer&& other);
  IndexBuffer& operator=(IndexBuffer&& other);

  ~IndexBuffer();

  void destroy() noexcept;

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::Mesh;

  IndexBuffer(Context& ctx, const size_t byte_length);
  IndexBuffer(Context& ctx, const uint16_t* const data_ptr, const size_t byte_length);

  void update(const uint16_t* const data_ptr, const size_t byte_length) noexcept;
};

}  // namespace crystal::opengl
