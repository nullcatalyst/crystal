#pragma once

#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class Mesh;

class VertexBuffer {
  Context*      ctx_               = nullptr;
  VkBuffer      buffer_            = VK_NULL_HANDLE;
  VmaAllocation buffer_allocation_ = VK_NULL_HANDLE;
  void*         ptr_               = nullptr;
  size_t        capacity_          = 0;

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
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::Mesh;

  VertexBuffer(Context& ctx, const size_t byte_length);
  VertexBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length);
};

}  // namespace crystal::vulkan
