#pragma once

#include <vector>

#include "absl/container/inlined_vector.h"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;
class RenderPass;

namespace internal {

class Swapchain {
  struct Image {
    VkImage     image;
    VkImageView view;
  };

  //   VkInstance       instance_        = VK_NULL_HANDLE;
  VkSurfaceKHR     surface_         = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice         device_          = VK_NULL_HANDLE;

  VkSwapchainKHR                swapchain_ = VK_NULL_HANDLE;
  absl::InlinedVector<Image, 4> images_;
  VkQueue                       graphics_queue_ = VK_NULL_HANDLE;
  VkQueue                       present_queue_  = VK_NULL_HANDLE;

  // Cached data used to recreate the swapchain (eg: on resize).
  union {
    struct {
      uint32_t graphics_queue_index_;
      uint32_t present_queue_index_;
    };
    uint32_t device_queues_[2];
  };

  VkSwapchainCreateInfoKHR create_info_ = {};

public:
  Swapchain() = default;
  ~Swapchain();

  [[nodiscard]] constexpr uint32_t screen_width() const { return create_info_.imageExtent.width; }
  [[nodiscard]] constexpr uint32_t screen_height() const { return create_info_.imageExtent.height; }

  void destroy();

  void init(Context& ctx, uint32_t graphics_queue_index, uint32_t present_queue_index);

  // Create should never be called directly, instead use recreate() to make sure that the old
  // swapchain image views get cleaned up.
  void create();
  void recreate();

  uint32_t acquire_next_image(VkSemaphore image_available_semaphore);

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;
  friend class ::crystal::vulkan::RenderPass;
};

}  // namespace internal

}  // namespace crystal::vulkan
