#pragma once

#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                                \
  VkDevice      device_           = VK_NULL_HANDLE;            \
  VmaAllocator  memory_allocator_ = VK_NULL_HANDLE;            \
  VmaAllocation allocation_       = VK_NULL_HANDLE;            \
  VkImage       image_            = VK_NULL_HANDLE;            \
  VkImageView   image_view_       = VK_NULL_HANDLE;            \
  VkSampler     sampler_          = VK_NULL_HANDLE;            \
  VkFormat      format_           = VK_FORMAT_UNDEFINED;       \
  VkImageLayout layout_           = VK_IMAGE_LAYOUT_UNDEFINED; \
  VkExtent2D    extent_           = {};
#define CRYSTAL_IMPL_CTOR Texture(Context& ctx, const TextureDesc& desc);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/texture.inl"
