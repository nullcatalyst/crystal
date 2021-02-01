#pragma once

#include <cstddef>

#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class Mesh;

class VertexBuffer {
  OBJC(MTLBuffer) buffer_ = nullptr;
  size_t capacity_        = 0;

public:
  constexpr VertexBuffer() = default;

  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer& operator=(const VertexBuffer&) = delete;

  VertexBuffer(VertexBuffer&& other);
  VertexBuffer& operator=(VertexBuffer&& other);

  ~VertexBuffer();

  void destroy() noexcept;

  void update(const void* const data_ptr, const size_t byte_length) noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::Mesh;

  VertexBuffer(OBJC(MTLDevice) device, const size_t byte_length);
  VertexBuffer(OBJC(MTLDevice) device, const void* const data_ptr, const size_t byte_length);
};

}  // namespace crystal::metal
