#include "crystal/metal/uniform_buffer.hpp"

#include "crystal/metal/context.hpp"
#include "util/msg/msg.hpp"

namespace crystal::metal {

UniformBuffer::UniformBuffer(UniformBuffer&& other)
    : buffer_(other.buffer_), capacity_(other.capacity_) {
  other.buffer_   = nullptr;
  other.capacity_ = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) {
  destroy();

  buffer_   = other.buffer_;
  capacity_ = other.capacity_;

  other.buffer_   = nullptr;
  other.capacity_ = 0;

  return *this;
}

UniformBuffer::~UniformBuffer() { destroy(); }

void UniformBuffer::destroy() noexcept {
  buffer_   = nullptr;
  capacity_ = 0;
}

void UniformBuffer::update(const void* const data_ptr, const size_t byte_length) noexcept {
  if (byte_length > capacity_) {
    util::msg::fatal("updating uniform buffer that has capacity [", capacity_,
                     "] with data that exceeds that capacity at length [", byte_length, "]");
  }

  memcpy([buffer_ contents], data_ptr, byte_length);
  [buffer_ didModifyRange:NSRange{0, byte_length}];
}

UniformBuffer::UniformBuffer(OBJC(MTLDevice) device, const size_t byte_length)
    : buffer_(nullptr), capacity_(byte_length) {
  buffer_ = [device newBufferWithLength:byte_length options:MTLResourceStorageModeShared];
}

UniformBuffer::UniformBuffer(OBJC(MTLDevice) device, const void* const data_ptr,
                             const size_t byte_length)
    : buffer_(nullptr), capacity_(byte_length) {
  buffer_ = [device newBufferWithBytes:data_ptr
                                length:byte_length
                               options:MTLResourceStorageModeShared];
  // update(data_ptr, byte_length);
}

}  // namespace crystal::metal
