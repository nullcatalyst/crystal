#pragma once

#include <array>
#include <initializer_list>
#include <tuple>

#include "absl/container/inlined_vector.h"
#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class CommandBuffer;
class IndexBuffer;
class VertexBuffer;

class Mesh {
  struct Binding {
    uint32_t index;
    OBJC(MTLBuffer) buffer;
  };

  uint32_t               binding_count_ = 0;
  std::array<Binding, 8> bindings_      = {};
  OBJC(MTLBuffer) index_buffer_         = nullptr;

public:
  constexpr Mesh() = default;

  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  ~Mesh();

  void destroy() noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::CommandBuffer;

  Mesh(Context&                                                               ctx,
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings);
  Mesh(Context&                                                               ctx,
       const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
       const IndexBuffer&                                                     index_buffer);
};

}  // namespace crystal::metal
