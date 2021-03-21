#pragma once

#include "crystal/common/texture_desc.hpp"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;
class RenderPass;

class Texture {
  VkDevice      device_           = VK_NULL_HANDLE;
  VmaAllocator  memory_allocator_ = VK_NULL_HANDLE;
  VmaAllocation allocation_       = VK_NULL_HANDLE;
  VkImage       image_            = VK_NULL_HANDLE;
  VkImageView   image_view_       = VK_NULL_HANDLE;
  VkSampler     sampler_          = VK_NULL_HANDLE;
  VkFormat      format_           = VK_FORMAT_UNDEFINED;
  VkImageLayout layout_           = VK_IMAGE_LAYOUT_UNDEFINED;
  VkExtent2D    extent_           = {};

public:
  constexpr Texture() = default;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other);
  Texture& operator=(Texture&& other);

  ~Texture();

  void destroy();

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;
  friend class ::crystal::vulkan::RenderPass;

  Texture(Context& ctx, const TextureDesc& desc);
};

}  // namespace crystal::vulkan
