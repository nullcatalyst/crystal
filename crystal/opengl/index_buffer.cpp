#include "crystal/opengl/index_buffer.hpp"

#include "crystal/opengl/context.hpp"

namespace crystal::opengl {

IndexBuffer::IndexBuffer(IndexBuffer&& other)
    : ctx_(other.ctx_), buffer_(other.buffer_), capacity_(other.capacity_) {
  other.ctx_      = nullptr;
  other.buffer_   = 0;
  other.capacity_ = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) {
  if (ctx_ != nullptr) {
    ctx_->release_buffer_(buffer_);
  }

  ctx_      = other.ctx_;
  buffer_   = other.buffer_;
  capacity_ = other.capacity_;

  other.ctx_      = nullptr;
  other.buffer_   = 0;
  other.capacity_ = 0;

  return *this;
}

IndexBuffer::~IndexBuffer() {
  if (ctx_ == nullptr) {
    return;
  }

  ctx_->release_buffer_(buffer_);
}

IndexBuffer::IndexBuffer(Context& ctx, const size_t byte_length)
    : ctx_(&ctx), buffer_(0), capacity_(byte_length) {
  GL_ASSERT(glGenBuffers(1, &buffer_), "generating index buffer");
  ctx_->add_buffer_(buffer_);

  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_), "binding index buffer");
  GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, byte_length, nullptr, GL_DYNAMIC_DRAW),
            "reserving index buffer capacity");
}

IndexBuffer::IndexBuffer(Context& ctx, const uint16_t* const data_ptr, const size_t byte_length)
    : ctx_(&ctx), buffer_(0), capacity_(byte_length) {
  GL_ASSERT(glGenBuffers(1, &buffer_), "generating index buffer");
  ctx_->add_buffer_(buffer_);

  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_), "binding index buffer");
  GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, byte_length, data_ptr, GL_DYNAMIC_DRAW),
            "updating index buffer data");
}

void IndexBuffer::update(const uint16_t* const data_ptr, const size_t byte_length) noexcept {
  if (byte_length > capacity_) {
    util::msg::fatal("updating index buffer that has capacity [", capacity_,
                     "] with data that exceeds that capacity at length [", byte_length, "]");
  }

  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_), "binding index buffer");
  GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, byte_length, data_ptr, GL_DYNAMIC_DRAW),
            "updating index buffer data");
}

}  // namespace crystal::opengl
