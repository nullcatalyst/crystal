#include "crystal/opengl/context.hpp"
#include "crystal/opengl/vertex_buffer.hpp"

namespace crystal::opengl {

UniformBuffer::UniformBuffer(UniformBuffer&& other)
    : ctx_(other.ctx_), buffer_(other.buffer_), capacity_(other.capacity_) {
  other.ctx_      = nullptr;
  other.buffer_   = 0;
  other.capacity_ = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) {
  destroy();

  ctx_      = other.ctx_;
  buffer_   = other.buffer_;
  capacity_ = other.capacity_;

  other.ctx_      = nullptr;
  other.buffer_   = 0;
  other.capacity_ = 0;

  return *this;
}

UniformBuffer::~UniformBuffer() { destroy(); }

void UniformBuffer::destroy() {
  if (ctx_ == nullptr) {
    return;
  }

  ctx_->release_buffer_(buffer_);

  ctx_      = nullptr;
  buffer_   = 0;
  capacity_ = 0;
}

UniformBuffer::UniformBuffer(Context& ctx, const size_t byte_length)
    : ctx_(&ctx), buffer_(0), capacity_(byte_length) {
  GL_ASSERT(glGenBuffers(1, &buffer_), "generating uniform buffer");
  ctx_->add_buffer_(buffer_);

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, buffer_), "binding uniform buffer");
  GL_ASSERT(glBufferData(GL_UNIFORM_BUFFER, byte_length, nullptr, GL_DYNAMIC_DRAW),
            "reserving uniform buffer capacity");
}

UniformBuffer::UniformBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length)
    : ctx_(&ctx), buffer_(0), capacity_(byte_length) {
  GL_ASSERT(glGenBuffers(1, &buffer_), "generating uniform buffer");
  ctx_->add_buffer_(buffer_);

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, buffer_), "binding uniform buffer");
  GL_ASSERT(glBufferData(GL_UNIFORM_BUFFER, byte_length, data_ptr, GL_DYNAMIC_DRAW),
            "updating uniform buffer data");
}

void UniformBuffer::update(const void* const data_ptr, const size_t byte_length) noexcept {
  if (byte_length > capacity_) {
    util::msg::fatal("updating uniform buffer that has capacity [", capacity_,
                     "] with data that exceeds that capacity at length [", byte_length, "]");
  }

  GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, buffer_), "binding uniform buffer");
  GL_ASSERT(glBufferData(GL_UNIFORM_BUFFER, byte_length, data_ptr, GL_DYNAMIC_DRAW),
            "updating uniform buffer data");
}

}  // namespace crystal::opengl
