#pragma once

#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                      \
  Context*      ctx_;                                \
  VkBuffer      buffer_            = VK_NULL_HANDLE; \
  VmaAllocation buffer_allocation_ = VK_NULL_HANDLE; \
  uint16_t*     ptr_               = nullptr;        \
  size_t        capacity_          = 0;
#define CRYSTAL_IMPL_CTOR                              \
  IndexBuffer(Context& ctx, const size_t byte_length); \
  IndexBuffer(Context& ctx, const uint16_t* const data_ptr, const size_t byte_length);
#define CRYSTAL_IMPL_METHODS \
  void update(const uint16_t* const data_ptr, const size_t byte_length) noexcept;
#include "crystal/interface/index_buffer.inl"
