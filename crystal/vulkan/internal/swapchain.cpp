#include "crystal/vulkan/internal/swapchain.hpp"

#include "crystal/vulkan/context.hpp"

namespace crystal::vulkan::internal {

Swapchain::~Swapchain() {
  // Wait until we are sure that the existing swapchain images are no longer in use.
  vkDeviceWaitIdle(device_);

  for (const auto& image : images_) {
    vkDestroyImageView(device_, image.view, nullptr);
  }
  images_.clear();

  vkDestroySwapchainKHR(device_, swapchain_, nullptr);
}

void Swapchain::destroy() {
  if (device_ == nullptr) {
    return;
  }

  // Wait until we are sure that the existing swapchain images are no longer in use.
  vkDeviceWaitIdle(device_);

  for (const auto& image : images_) {
    vkDestroyImageView(device_, image.view, nullptr);
  }
  images_.clear();

  vkDestroySwapchainKHR(device_, swapchain_, nullptr);

  surface_         = VK_NULL_HANDLE;
  physical_device_ = VK_NULL_HANDLE;
  device_          = VK_NULL_HANDLE;
  swapchain_       = VK_NULL_HANDLE;
  images_.resize(0);
  graphics_queue_       = VK_NULL_HANDLE;
  present_queue_        = VK_NULL_HANDLE;
  graphics_queue_index_ = 0;
  present_queue_index_  = 0;
  create_info_          = {};
}

void Swapchain::init(Context& ctx, uint32_t graphics_queue_index, uint32_t present_queue_index) {
  surface_              = ctx.surface_;
  physical_device_      = ctx.physical_device_;
  device_               = ctx.device_;
  swapchain_            = VK_NULL_HANDLE;
  graphics_queue_index_ = graphics_queue_index;
  present_queue_index_  = present_queue_index;

  vkGetDeviceQueue(device_, graphics_queue_index, 0, &graphics_queue_);
  vkGetDeviceQueue(device_, present_queue_index, 0, &present_queue_);

  create();
}

void Swapchain::create() {
  {  // Create swapchain.
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_, surface_,
                                                        &surface_capabilities),
              "getting surface capabilities");

    uint32_t surface_format_count = 0;
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface_,
                                                   &surface_format_count, nullptr),
              "getting surface formats");
    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface_,
                                                   &surface_format_count, surface_formats.data()),
              "getting surface formats");

    uint32_t present_mode_count = 0;
    VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface_,
                                                        &present_mode_count, nullptr),
              "getting surface present modes");
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface_,
                                                        &present_mode_count, present_modes.data()),
              "getting surface present modes");

    uint32_t      queue_family_index_count = 0;
    uint32_t*     queue_family_indices     = nullptr;
    VkSharingMode sharing_mode             = VK_SHARING_MODE_EXCLUSIVE;
    if (graphics_queue_index_ != present_queue_index_) {
      queue_family_index_count = 2;
      queue_family_indices     = device_queues_;
      sharing_mode             = VK_SHARING_MODE_CONCURRENT;
    }

    uint32_t surface_format_index = 0;
    for (int i = 0; i < surface_formats.size(); ++i) {
      // util::msg::info("surface_format.format=", surface_format.format);
      // util::msg::info("surface_format.colorSpace=", surface_format.colorSpace);
      if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB) {
        surface_format_index = i;
        break;
      }
    }

    uint32_t present_mode_index = 0;
    for (int i = 0; i < present_modes.size(); ++i) {
      if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        present_mode_index = i;
        break;
      }
    }

    create_info_ = {
        /* .sType = */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        /* .pNext                 = */ nullptr,
        /* .flags                 = */ 0,
        /* .surface               = */ surface_,
        /* .minImageCount         = */ 2,
        /* .imageFormat           = */ surface_formats[surface_format_index].format,
        /* .imageColorSpace       = */ surface_formats[surface_format_index].colorSpace,
        /* .imageExtent           = */ surface_capabilities.currentExtent,
        /* .imageArrayLayers      = */ 1,
        /* .imageUsage            = */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        /* .imageSharingMode      = */ sharing_mode,
        /* .queueFamilyIndexCount = */ queue_family_index_count,
        /* .pQueueFamilyIndices   = */ queue_family_indices,
        /* .preTransform          = */ surface_capabilities.currentTransform,
        /* .compositeAlpha        = */ VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        /* .presentMode           = */ present_modes[present_mode_index],
        /* .clipped               = */ VK_TRUE,
        /* .oldSwapchain          = */ swapchain_,
    };

    VK_ASSERT(vkCreateSwapchainKHR(device_, &create_info_, nullptr, &swapchain_),
              "creating swapchain");

    // Next, the swapchain images need to be cached.
    uint32_t image_count = 0;
    VK_ASSERT(vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, nullptr),
              "getting swapchain images");
    std::vector<VkImage> images(image_count);
    VK_ASSERT(vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, images.data()),
              "getting swapchain images");

    images_.resize(image_count);
    for (uint32_t i = 0; i < image_count; ++i) {
      {  // Create swapchain image view.
        const VkImageViewCreateInfo create_info = {
            /* .sType = */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            /* .pNext            = */ nullptr,
            /* .flags            = */ 0,
            /* .image            = */ images[i],
            /* .viewType         = */ VK_IMAGE_VIEW_TYPE_2D,
            /* .format           = */ create_info_.imageFormat,
            /* .components       = */
            {
                /* .r = */ VK_COMPONENT_SWIZZLE_IDENTITY,
                /* .g = */ VK_COMPONENT_SWIZZLE_IDENTITY,
                /* .b = */ VK_COMPONENT_SWIZZLE_IDENTITY,
                /* .a = */ VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            /* .subresourceRange = */
            {
                /* .aspectMask     = */ VK_IMAGE_ASPECT_COLOR_BIT,
                /* .baseMipLevel   = */ 0,
                /* .levelCount     = */ 1,
                /* .baseArrayLayer = */ 0,
                /* .layerCount     = */ 1,
            },
        };

        images_[i].image = images[i];
        VK_ASSERT(vkCreateImageView(device_, &create_info, nullptr, &images_[i].view),
                  "creating swapchain image view");
      }
    }
  }
}

void Swapchain::recreate() {
  // Wait until we are sure that the existing swapchain images are no longer in use.
  vkDeviceWaitIdle(device_);

  for (const auto& image : images_) {
    vkDestroyImageView(device_, image.view, nullptr);
  }
  images_.clear();

  create();
}

uint32_t Swapchain::acquire_next_image(VkSemaphore image_available_semaphore) {
  uint32_t next_image_index = 0;
  VK_ASSERT(vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, image_available_semaphore,
                                  nullptr, &next_image_index),
            "acquiring next swapchain image");
  return next_image_index;
}

void Swapchain::present(VkSemaphore rendering_complete_semaphore, uint32_t image_index) {
#ifdef __ggp__
  const VkPresentFrameTokenGGP frame_token_metadata = {
      /* sType */ VK_STRUCTURE_TYPE_PRESENT_FRAME_TOKEN_GGP,
      /* pNext      */ nullptr,
      /* frameToken */ frame_token_,
  };

  const VkPresentInfoKHR present_info = {
      /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      /* pNext              */ &frame_token_metadata,
      /* waitSemaphoreCount */ 1,
      /* pWaitSemaphores    */ &rendering_complete_semaphore,
      /* swapchainCount     */ 1,
      /* pSwapchains        */ &swapchain_,
      /* pImageIndices      */ &image_index,
      /* pResults           */ nullptr,
  };
#else
  const VkPresentInfoKHR present_info = {
      /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      /* pNext              */ nullptr,
      /* waitSemaphoreCount */ 1,
      /* pWaitSemaphores    */ &rendering_complete_semaphore,
      /* swapchainCount     */ 1,
      /* pSwapchains        */ &swapchain_,
      /* pImageIndices      */ &image_index,
      /* pResults           */ nullptr,
  };
#endif  // __ggp__

  VK_ASSERT(vkQueuePresentKHR(present_queue_, &present_info), "presenting queue");
}

}  // namespace crystal::vulkan::internal
