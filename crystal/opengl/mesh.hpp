#pragma once

#include <initializer_list>
#include <tuple>

#include "absl/container/inlined_vector.h"
#include "crystal/opengl/gl.hpp"

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES                                                   \
  struct Vao {                                                                    \
    uint32_t pipeline_id;                                                         \
    GLuint   vao;                                                                 \
                                                                                  \
    ~Vao() { GL_ASSERT(glDeleteVertexArrays(1, &vao), "deleting vertex array"); } \
  };                                                                              \
                                                                                  \
  struct Binding {                                                                \
    uint32_t buffer_index;                                                        \
    GLuint   vertex_buffer;                                                       \
  };                                                                              \
                                                                                  \
  Context*                        ctx_;                                           \
  absl::InlinedVector<Vao, 4>     vaos_;                                          \
  absl::InlinedVector<Binding, 4> bindings_;                                      \
  GLuint                          index_buffer_;
#define CRYSTAL_IMPL_CTOR                                                                \
  Mesh(Context&                                                               ctx,       \
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings); \
  Mesh(Context&                                                               ctx,       \
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,  \
       const IndexBuffer&                                                     index_buffer);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/mesh.inl"
