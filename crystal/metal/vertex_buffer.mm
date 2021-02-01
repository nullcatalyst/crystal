#include "crystal/metal/vertex_buffer.hpp"

#include "crystal/metal/context.hpp"
#include "util/msg/msg.hpp"

namespace crystal::metal {

VertexBuffer::VertexBuffer(VertexBuffer&& other)
    : buffer_(other.buffer_), capacity_(other.capacity_) {
  other.buffer_   = nullptr;
  other.capacity_ = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) {
  destroy();

  buffer_   = other.buffer_;
  capacity_ = other.capacity_;

  other.buffer_   = nullptr;
  other.capacity_ = 0;

  return *this;
}

VertexBuffer::~VertexBuffer() { destroy(); }

void VertexBuffer::destroy() noexcept {
  buffer_   = nullptr;
  capacity_ = 0;
}

void VertexBuffer::update(const void* const data_ptr, const size_t byte_length) noexcept {
  if (byte_length > capacity_) {
    util::msg::fatal("updating vertex buffer that has capacity [", capacity_,
                     "] with data that exceeds that capacity at length [", byte_length, "]");
  }

  memcpy([buffer_ contents], data_ptr, byte_length);
  [buffer_ didModifyRange:NSRange{0, byte_length}];
}

VertexBuffer::VertexBuffer(OBJC(MTLDevice) device, const size_t byte_length)
    : buffer_(nullptr), capacity_(byte_length) {
  buffer_ = [device newBufferWithLength:byte_length options:MTLResourceStorageModeManaged];
}

VertexBuffer::VertexBuffer(OBJC(MTLDevice) device, const void* const data_ptr,
                           const size_t byte_length)
    : buffer_(nullptr), capacity_(byte_length) {
  buffer_ = [device newBufferWithBytes:data_ptr
                                length:byte_length
                               options:MTLResourceStorageModeManaged];
  // update(data_ptr, byte_length);
}

}  // namespace crystal::metal
