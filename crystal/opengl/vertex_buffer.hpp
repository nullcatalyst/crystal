#pragma once

#include "crystal/opengl/gl.hpp"

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES \
  Context* ctx_;                \
  GLuint   buffer_;             \
  size_t   capacity_ = 0;
#define CRYSTAL_IMPL_CTOR                               \
  VertexBuffer(Context& ctx, const size_t byte_length); \
  VertexBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length);
#define CRYSTAL_IMPL_METHODS \
  void update(const void* const data_ptr, const size_t byte_length) noexcept;
#include "crystal/interface/vertex_buffer.inl"
