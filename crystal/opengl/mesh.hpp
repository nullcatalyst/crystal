#pragma once

#include <initializer_list>
#include <tuple>

#include "absl/container/inlined_vector.h"
#include "crystal/opengl/gl.hpp"

namespace crystal::opengl {

class Context;
class CommandBuffer;
class IndexBuffer;
class VertexBuffer;

class Mesh {
  struct Vao {
    uint32_t pipeline_id;
    GLuint   vao;

    ~Vao();
  };

  struct Binding {
    uint32_t buffer_index;
    GLuint   vertex_buffer;
  };

  Context*                        ctx_          = nullptr;
  absl::InlinedVector<Vao, 4>     vaos_         = {};
  absl::InlinedVector<Binding, 4> bindings_     = {};
  GLuint                          index_buffer_ = 0;

public:
  constexpr Mesh() = default;

  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  ~Mesh();

  void destroy() noexcept;

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::CommandBuffer;

  Mesh(Context&                                                               ctx,
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings);
  Mesh(Context&                                                               ctx,
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
       const IndexBuffer&                                                     index_buffer);
};

}  // namespace crystal::opengl
