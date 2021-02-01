#pragma once

#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class Mesh;

class IndexBuffer {
  OBJC(MTLBuffer) buffer_ = nullptr;
  size_t capacity_        = 0;

public:
  constexpr IndexBuffer() = default;

  IndexBuffer(const IndexBuffer&) = delete;
  IndexBuffer& operator=(const IndexBuffer&) = delete;

  IndexBuffer(IndexBuffer&& other);
  IndexBuffer& operator=(IndexBuffer&& other);

  ~IndexBuffer();

  void destroy() noexcept;

  void update(const uint16_t* const data_ptr, const size_t byte_length) noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::Mesh;

  IndexBuffer(OBJC(MTLDevice) device, const size_t byte_length);
  IndexBuffer(OBJC(MTLDevice) device, const uint16_t* const data_ptr, const size_t byte_length);
};

}  // namespace crystal::metal
