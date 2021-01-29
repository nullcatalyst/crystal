#pragma once

#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;

class UniformBuffer {
  Context*      ctx_               = nullptr;
  VkBuffer      buffer_            = VK_NULL_HANDLE;
  VmaAllocation buffer_allocation_ = VK_NULL_HANDLE;
  void*         ptr_               = nullptr;
  size_t        capacity_          = 0;

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
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;

  UniformBuffer(Context& ctx, const size_t byte_length);
  UniformBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length);
};

}  // namespace crystal::vulkan
