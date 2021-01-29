#pragma once

#include <algorithm>

#include "absl/container/inlined_vector.h"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;

namespace internal {

class Frame {
  VkDevice        device_                = VK_NULL_HANDLE;
  VkCommandBuffer command_buffer_        = VK_NULL_HANDLE;
  uint32_t        swapchain_image_index_ = 0;

  // Used to ensure that the previous frame is finished rendering before reuse.
  VkSemaphore image_available_semaphore_    = VK_NULL_HANDLE;
  VkSemaphore rendering_complete_semaphore_ = VK_NULL_HANDLE;
  VkFence     fence_                        = VK_NULL_HANDLE;

public:
  Frame() = default;
  ~Frame();

  void init(Context& ctx);

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;
};

}  // namespace internal

}  // namespace crystal::vulkan
