#pragma once

#include <cstddef>

#include "crystal/opengl/gl.hpp"

namespace crystal::opengl {

class Context;
class CommandBuffer;

class UniformBuffer {
  Context* ctx_      = nullptr;
  GLuint   buffer_   = 0;
  size_t   capacity_ = 0;

public:
  constexpr UniformBuffer() = default;

  UniformBuffer(const UniformBuffer&) = delete;
  UniformBuffer& operator=(const UniformBuffer&) = delete;

  UniformBuffer(UniformBuffer&& other);
  UniformBuffer& operator=(UniformBuffer&& other);

  ~UniformBuffer();

  void destroy() noexcept;

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::CommandBuffer;

  UniformBuffer(Context& ctx, const size_t byte_length);
  UniformBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length);

  void update(const void* const data_ptr, const size_t byte_length) noexcept;
};

}  // namespace crystal::opengl
