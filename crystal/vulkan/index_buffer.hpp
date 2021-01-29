#pragma once

#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class Mesh;

class IndexBuffer {
  Context*      ctx_               = nullptr;
  VkBuffer      buffer_            = VK_NULL_HANDLE;
  VmaAllocation buffer_allocation_ = VK_NULL_HANDLE;
  uint16_t*     ptr_               = nullptr;
  size_t        capacity_          = 0;

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
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::Mesh;

  IndexBuffer(Context& ctx, const size_t byte_length);
  IndexBuffer(Context& ctx, const uint16_t* const data_ptr, const size_t byte_length);
};

}  // namespace crystal::vulkan
