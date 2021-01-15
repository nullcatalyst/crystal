#pragma once

#include <cstdint>

#include "crystal/vulkan/internal/frame.hpp"
#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                                         \
  VkCommandBuffer  command_buffer_               = VK_NULL_HANDLE;      \
  VkImage          swapchain_image_              = VK_NULL_HANDLE;      \
  VkImageView      swapchain_image_view_         = VK_NULL_HANDLE;      \
  VkFormat         swapchain_image_format_       = VK_FORMAT_UNDEFINED; \
  VkExtent2D       swapchain_image_extent_       = {};                  \
  VkDevice         device_                       = VK_NULL_HANDLE;      \
  VkSwapchainKHR   swapchain_                    = VK_NULL_HANDLE;      \
  VkQueue          graphics_queue_               = VK_NULL_HANDLE;      \
  VkQueue          present_queue_                = VK_NULL_HANDLE;      \
  VkSemaphore      image_available_semaphore_    = VK_NULL_HANDLE;      \
  VkSemaphore      rendering_complete_semaphore_ = VK_NULL_HANDLE;      \
  VkFence          fence_                        = VK_NULL_HANDLE;      \
  uint32_t         swapchain_image_index_        = 0;                   \
  uint32_t         frame_index_                  = 0;                   \
  VkPipelineLayout pipeline_layout_              = VK_NULL_HANDLE;      \
  VkDescriptorSet  descriptor_set_               = VK_NULL_HANDLE;      \
  bool             update_descriptor_set_        = false;
#define CRYSTAL_IMPL_CTOR CommandBuffer(Context& ctx, internal::Frame& frame, uint32_t frame_index);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/command_buffer.inl"
