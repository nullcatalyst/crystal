#include "crystal/opengl/vertex_buffer.hpp"

#include "crystal/opengl/context.hpp"

namespace crystal::opengl {

VertexBuffer::VertexBuffer(VertexBuffer&& other)
    : ctx_(other.ctx_), buffer_(other.buffer_), capacity_(other.capacity_) {
  other.ctx_      = nullptr;
  other.buffer_   = 0;
  other.capacity_ = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) {
  destroy();

  ctx_      = other.ctx_;
  buffer_   = other.buffer_;
  capacity_ = other.capacity_;

  other.ctx_      = nullptr;
  other.buffer_   = 0;
  other.capacity_ = 0;

  return *this;
}

VertexBuffer::~VertexBuffer() { destroy(); }

void VertexBuffer::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  ctx_->release_buffer_(buffer_);

  ctx_      = nullptr;
  buffer_   = 0;
  capacity_ = 0;
}

void VertexBuffer::update(const void* const data_ptr, const size_t byte_length) noexcept {
  if (byte_length > capacity_) {
    util::msg::fatal("updating vertex buffer that has capacity [", capacity_,
                     "] with data that exceeds that capacity at length [", byte_length, "]");
  }

  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, buffer_), "binding vertex buffer");
  GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, byte_length, data_ptr, GL_DYNAMIC_DRAW),
            "updating vertex buffer data");
}

VertexBuffer::VertexBuffer(Context& ctx, const size_t byte_length)
    : ctx_(&ctx), buffer_(0), capacity_(byte_length) {
  GL_ASSERT(glGenBuffers(1, &buffer_), "generating vertex buffer");
  ctx_->add_buffer_(buffer_);

  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, buffer_), "binding vertex buffer");
  GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, byte_length, nullptr, GL_DYNAMIC_DRAW),
            "reserving vertex buffer capacity");
}

VertexBuffer::VertexBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length)
    : ctx_(&ctx), buffer_(0), capacity_(byte_length) {
  GL_ASSERT(glGenBuffers(1, &buffer_), "generating vertex buffer");
  ctx_->add_buffer_(buffer_);

  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, buffer_), "binding vertex buffer");
  GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, byte_length, data_ptr, GL_DYNAMIC_DRAW),
            "updating vertex buffer data");
}

}  // namespace crystal::opengl
