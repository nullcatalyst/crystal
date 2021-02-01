#pragma once

#include <cstddef>

#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class CommandBuffer;

class UniformBuffer {
  OBJC(MTLBuffer) buffer_ = nullptr;
  size_t capacity_        = 0;

public:
  constexpr UniformBuffer() = default;

  UniformBuffer(const UniformBuffer&) = delete;
  UniformBuffer& operator=(const UniformBuffer&) = delete;

  UniformBuffer(UniformBuffer&& other);
  UniformBuffer& operator=(UniformBuffer&& other);

  ~UniformBuffer();

  void destroy() noexcept;

  void update(const void* const data_ptr, const size_t byte_length) noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::CommandBuffer;

  UniformBuffer(OBJC(MTLDevice) device, const size_t byte_length);
  UniformBuffer(OBJC(MTLDevice) device, const void* const data_ptr, const size_t byte_length);
};

}  // namespace crystal::metal
