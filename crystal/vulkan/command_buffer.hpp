#pragma once

#include <cstdint>

#include "crystal/vulkan/internal/frame.hpp"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class Mesh;
class Pipeline;
class RenderPass;
class Texture;
class UniformBuffer;

class CommandBuffer {
  VkCommandBuffer  command_buffer_                = VK_NULL_HANDLE;
  VkImage          swapchain_image_               = VK_NULL_HANDLE;
  VkImageView      swapchain_image_view_          = VK_NULL_HANDLE;
  VkFormat         swapchain_image_format_        = VK_FORMAT_UNDEFINED;
  VkExtent2D       swapchain_image_extent_        = {};
  VkDevice         device_                        = VK_NULL_HANDLE;
  VkSwapchainKHR   swapchain_                     = VK_NULL_HANDLE;
  VkQueue          graphics_queue_                = VK_NULL_HANDLE;
  VkQueue          present_queue_                 = VK_NULL_HANDLE;
  VkSemaphore      image_available_semaphore_     = VK_NULL_HANDLE;
  VkSemaphore      rendering_complete_semaphore_  = VK_NULL_HANDLE;
  VkFence          fence_                         = VK_NULL_HANDLE;
  uint32_t         swapchain_image_index_         = 0;
  uint32_t         frame_index_                   = 0;
  VkPipelineLayout pipeline_layout_               = VK_NULL_HANDLE;
  VkDescriptorSet  uniform_descriptor_set_        = VK_NULL_HANDLE;
  VkDescriptorSet  texture_descriptor_set_        = VK_NULL_HANDLE;
  bool             update_uniform_descriptor_set_ = false;
  bool             update_texture_descriptor_set_ = false;
  bool             in_render_pass_                = false;

public:
  CommandBuffer() = delete;

  CommandBuffer(const CommandBuffer&) = delete;
  CommandBuffer& operator=(const CommandBuffer&) = delete;

  CommandBuffer(CommandBuffer&&) = delete;
  CommandBuffer& operator=(CommandBuffer&&) = delete;

  ~CommandBuffer();

  void use_render_pass(const RenderPass& render_pass);
  void use_pipeline(const Pipeline& pipeline);
  void use_uniform_buffer(const UniformBuffer& uniform_buffer, uint32_t binding);
  void use_texture(const Texture& texture, uint32_t binding);

  void draw(const Mesh& mesh, uint32_t vertex_or_index_count, uint32_t instance_count);

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::RenderPass;

  CommandBuffer(Context& ctx, internal::Frame& frame, uint32_t frame_index);
};

}  // namespace crystal::vulkan
