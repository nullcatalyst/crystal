#include "crystal/vulkan/internal/frame.hpp"

#include "crystal/vulkan/context.hpp"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan::internal {

Frame::~Frame() {
  vkDestroyFence(device_, fence_, nullptr);
  vkDestroySemaphore(device_, rendering_complete_semaphore_, nullptr);
  vkDestroySemaphore(device_, image_available_semaphore_, nullptr);
  // vkFreeCommandBuffers(device_, command_pool, 1, &command_buffer_);
}

void Frame::init(Context& ctx) {
  device_ = ctx.device_;

  {  // Allocate the command buffer.
    const VkCommandBufferAllocateInfo allocate_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* .pNext              = */ nullptr,
        /* .commandPool        = */ ctx.command_pool_,
        /* .level              = */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* .commandBufferCount = */ 1,
    };
    VK_ASSERT(vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer_),
              "allocating command buffer");
  }

  {  // Create sychronization primitives.
    const VkSemaphoreCreateInfo semaphore_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        /* .pNext = */ nullptr,
        /* .flags = */ 0,
    };
    VK_ASSERT(
        vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &image_available_semaphore_),
        "creating image available semaphore");
    VK_ASSERT(
        vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &rendering_complete_semaphore_),
        "creating rendering complete semaphore");

    const VkFenceCreateInfo fence_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        /* .pNext = */ nullptr,
        /* .flags = */ VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VK_ASSERT(vkCreateFence(device_, &fence_create_info, nullptr, &fence_), "creating fence");
  }
}

}  // namespace crystal::vulkan::internal
