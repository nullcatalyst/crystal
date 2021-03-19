#include "crystal/vulkan/context.hpp"

#include <array>
#include <vector>

#include "SDL_vulkan.h"
#include "util/msg/msg.hpp"

namespace {

const std::array<const char* const, 1> INSTANCE_LAYERS{
    // "VK_LAYER_LUNARG_standard_validation",
    "VK_LAYER_KHRONOS_validation",  // Someday... :sigh:
};

const std::array<const char* const, 2> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
};

}  // namespace

namespace crystal::vulkan {

#ifdef CRYSTAL_USE_SDL2

Context::Context(const Context::Desc& desc) {
  int width  = 0;
  int height = 0;
  SDL_GetWindowSize(desc.window, &width, &height);
  if (width < 0 || height < 0) {
    util::msg::fatal("window size is negative [", width, ", ", height, "]");
  }

  // width_  = width;
  // height_ = height;

  {  // Create instance.
    uint32_t instance_extension_count = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(desc.window, &instance_extension_count, nullptr)) {
      util::msg::fatal("getting SDL vulkan extensions: ", SDL_GetError());
    }
    std::vector<const char*> instance_extensions(instance_extension_count);
    if (!SDL_Vulkan_GetInstanceExtensions(desc.window, &instance_extension_count,
                                          instance_extensions.data())) {
      util::msg::fatal("getting SDL vulkan extensions: %s", SDL_GetError());
    }

    const VkApplicationInfo app_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
        /* .pNext              = */ nullptr,
        /* .pApplicationName   = */ desc.application_name,
        /* .applicationVersion = */ VK_MAKE_VERSION(1, 0, 0),
        /* .pEngineName        = */ "crystal",
        /* .engineVersion      = */ VK_MAKE_VERSION(0, 1, 0),
        /* .apiVersion         = */ VK_API_VERSION_1_0,
    };
    const VkInstanceCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        /* .pNext                   = */ nullptr,
        /* .flags                   = */ 0,
        /* .pApplicationInfo        = */ &app_info,
        /* .enabledLayerCount       = */ INSTANCE_LAYERS.size(),
        /* .ppEnabledLayerNames     = */ INSTANCE_LAYERS.data(),
        /* .enabledExtensionCount   = */ instance_extension_count,
        /* .ppEnabledExtensionNames = */ instance_extensions.data(),
    };

    VK_ASSERT(vkCreateInstance(&create_info, nullptr, &instance_), "creating instance");
  }

  {  // Create surface.
    if (!SDL_Vulkan_CreateSurface(desc.window, instance_, &surface_)) {
      util::msg::fatal("creating SDL vulkan surface: ", SDL_GetError());
    }
  }

  {  // Get physical device.
    uint32_t physical_device_count = 0;
    VK_ASSERT(vkEnumeratePhysicalDevices(instance_, &physical_device_count, nullptr),
              "getting physical device count");

    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    VK_ASSERT(
        vkEnumeratePhysicalDevices(instance_, &physical_device_count, physical_devices.data()),
        "listing physical devices");
    physical_device_ = physical_devices[0];
  }

  union {
    struct {
      uint32_t graphics_queue_index;
      uint32_t present_queue_index;
    };
    uint32_t device_queues[2];
  };
  graphics_queue_index = UINT32_MAX;
  present_queue_index  = UINT32_MAX;
  {  // Get device queues.

    uint32_t queue_property_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_property_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_properties(queue_property_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_property_count,
                                             queue_properties.data());
    for (uint32_t i = 0; i < queue_property_count; ++i) {
      const VkQueueFamilyProperties* queue_property = &queue_properties[i];
      if (queue_property->queueCount > 0) {
        // If this queue supports graphics, cache the index of the queue.
        if (queue_property->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          graphics_queue_index = i;
        }
        // Use the function pointer retrieved from the instance above to query
        // 'Present' support.
        VkBool32 supports_present = VK_FALSE;
        VK_ASSERT(
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, i, surface_, &supports_present),
            "getting physical device surface support");
        if (supports_present == VK_TRUE) {
          present_queue_index = i;
        }
        if (graphics_queue_index >= 0 && present_queue_index >= 0) {
          break;
        }
      }
    }
  }

  {  // Create device.
    const float             queue_priorities[]    = {1.0f};
    VkDeviceQueueCreateInfo queue_create_infos[2] = {
        {
            /* .sType = */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            /* .pNext            = */ nullptr,
            /* .flags            = */ 0,
            /* .queueFamilyIndex = */ graphics_queue_index,
            /* .queueCount       = */ 1,
            /* .pQueuePriorities = */ queue_priorities,
        },
        {
            /* .sType = */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            /* .pNext            = */ nullptr,
            /* .flags            = */ 0,
            /* .queueFamilyIndex = */ present_queue_index,
            /* .queueCount       = */ 1,
            /* .pQueuePriorities = */ queue_priorities,
        },
    };

    const VkDeviceCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        /* .pNext                   = */ nullptr,
        /* .flags                   = */ 0,
        /* .queueCreateInfoCount    = */ 1 +
            static_cast<uint32_t>(graphics_queue_index != present_queue_index),
        /* .pQueueCreateInfos       = */ queue_create_infos,
        /* .enabledLayerCount       = */ 0,
        /* .ppEnabledLayerNames     = */ nullptr,
        /* .enabledExtensionCount   = */ DEVICE_EXTENSIONS.size(),
        /* .ppEnabledExtensionNames = */ DEVICE_EXTENSIONS.data(),
        /* .pEnabledFeatures        = */ nullptr,
    };

    VK_ASSERT(vkCreateDevice(physical_device_, &create_info, nullptr, &device_), "creating device");
  }

  {  // Create memory allocator.
    const VmaAllocatorCreateInfo create_info = {
        /* .flags                       = */ 0,
        /* .physicalDevice              = */ physical_device_,
        /* .device                      = */ device_,
        /* .preferredLargeHeapBlockSize = */ 0,  // Use the default, which is currently 256 MiB.
        /* .pAllocationCallbacks        = */ nullptr,
        /* .pDeviceMemoryCallbacks      = */ nullptr,
        /* .frameInUseCount             = */ 3,
        /* .pHeapSizeLimit              = */ nullptr,
        /* .pVulkanFunctions            = */ nullptr,
        /* .pRecordSettings             = */ nullptr,
    };

    VK_ASSERT(vmaCreateAllocator(&create_info, &memory_allocator_), "creating memory allocator");
  }

  {  // Create command pool.
    const VkCommandPoolCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /* .pNext            = */ nullptr,
        /* .flags            = */ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        /* .queueFamilyIndex = */ graphics_queue_index,
    };

    VK_ASSERT(vkCreateCommandPool(device_, &create_info, nullptr, &command_pool_),
              "creating command pool");
  }

#if 0
  if (desc.pipeline_cache_name != nullptr) {  // Create pipeline cache.
      const VkPipelineCacheCreateInfo create_info = {
          /* sType */ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
          /* pNext           */ nullptr,
          /* flags           */ 0,
          /* initialDataSize */ 0,
          /* pInitialData    */ nullptr,
      };

      VK_ASSERT(vkCreatePipelineCache(device_, &create_info, nullptr, &pipeline_cache_),
                "creating pipeline cache");

      // Store the pipeline cache name so that we can save it to disk when we are
      // done.
      pipeline_cache_name_ = desc.pipeline_cache_name;
  }
#endif  // 0

  {  // Create descriptor pool.
    const VkDescriptorPoolSize pool_sizes[] = {
        VkDescriptorPoolSize{
            /* .type            = */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /* .descriptorCount = */ desc.buffer_descriptor_count,
        },
        VkDescriptorPoolSize{
            /* .type            = */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            /* .descriptorCount = */ desc.texture_descriptor_count,
        },
    };

    const VkDescriptorPoolCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        /* .pNext         = */ nullptr,
        /* .flags         = */ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        /* .maxSets       = */ desc.max_descriptor_set_count,
        /* .poolSizeCount = */ 2,
        /* .pPoolSizes    = */ pool_sizes,
    };

    VK_ASSERT(vkCreateDescriptorPool(device_, &create_info, nullptr, &descriptor_pool_),
              "creating descriptor pool");
  }

  swapchain_.init(*this, graphics_queue_index, present_queue_index);
  for (uint32_t i = 0; i < 4; ++i) {
    frames_[i].init(*this);
  }

  // {
  //   // All depth formats may be optional, so we need to find a suitable depth format to use
  //   const VkFormat depth_formats[] = {
  //       // VK_FORMAT_D32_SFLOAT_S8_UINT,
  //       VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT,
  //       VK_FORMAT_D16_UNORM_S8_UINT,  VK_FORMAT_D16_UNORM,
  //   };
  //   for (auto& format : depth_formats) {
  //     VkFormatProperties formatProperties;
  //     vkGetPhysicalDeviceFormatProperties(physical_device_, format, &formatProperties);
  //     // Format must support depth stencil attachment for optimal tiling
  //     if (formatProperties.optimalTilingFeatures &
  //     VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
  //       // if (checkSamplingSupport) {
  //       //   if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
  //       {
  //       //     continue;
  //       //   }
  //       // }
  //       util::msg::debug("depth format=", format);
  //       break;
  //     }
  //   }
  //   // util::msg::fatal("no usable depth format");
  // }

  screen_depth_texture_ = Texture(*this, TextureDesc{
                                             /* .width  = */ static_cast<uint32_t>(width),
                                             /* .height = */ static_cast<uint32_t>(height),
                                             /* .format = */ TextureFormat::Depth32f,
                                             /* .sample = */ TextureSample::Nearest,
                                             /* .repeat = */ TextureRepeat::Clamp,
                                         });
  screen_render_pass_   = RenderPass(*this);
}

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

Context::~Context() {
  if (buffers_.size() != 0) {
    util::msg::fatal("not all shared buffers have been released (there are still ", buffers_.size(),
                     " remaining), leaking memory");
  }
}

void Context::change_resolution(uint32_t width, uint32_t height) {
  util::msg::info("resolution size changed to ", width, ", ", height);

  // swapchain_.recreate();
  // TODO: Any existing framebuffers stored in RenderPass objects need to be updated if they contain
  // an old image view from the old swapchain.
}

void Context::set_clear_color(RenderPass& render_pass, uint32_t attachment,
                              ClearValue clear_value) {
  if (attachment >= render_pass.attachment_count_) {
    util::msg::fatal("setting clear color for out of bounds attachment [", attachment, "]");
  }
  render_pass.clear_values_[attachment].color = {
      clear_value.color.red,
      clear_value.color.green,
      clear_value.color.blue,
      clear_value.color.alpha,
  };
}

void Context::set_clear_depth(RenderPass& render_pass, ClearValue clear_value) {
  // if (!render_pass.has_depth_) {
  //   util::msg::fatal(
  //       "setting clear depth for render pass that does not contain a depth attachment");
  // }
  // render_pass.clear_depth_.clear_value = clear_value;
}

CommandBuffer Context::next_frame() {
  frame_index_                   = (frame_index_ + 1) & 3;
  auto&           frame          = frames_[frame_index_];
  VkCommandBuffer command_buffer = frame.command_buffer_;

  frame.swapchain_image_index_ = swapchain_.acquire_next_image(frame.image_available_semaphore_);

  VK_ASSERT(vkWaitForFences(device_, 1, &frame.fence_, VK_TRUE, UINT64_MAX), "waiting for fence");
  VK_ASSERT(vkResetFences(device_, 1, &frame.fence_), "resetting fences");

  {  // Begin command buffer.
    VK_ASSERT(vkResetCommandBuffer(command_buffer, 0), "resetting command buffer");

    const VkCommandBufferBeginInfo begin_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /* .pNext            = */ nullptr,
        /* .flags            = */ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /* .pInheritanceInfo = */ nullptr,
    };

    VK_ASSERT(vkBeginCommandBuffer(command_buffer, &begin_info), "beginning command buffer");
  }

  return CommandBuffer(*this, frame, frame_index_);
}

void Context::add_buffer_(VkBuffer buffer, VmaAllocation allocation) noexcept {
  buffers_.emplace_back(buffer, allocation);
}

void Context::retain_buffer_(VkBuffer buffer) noexcept {
  auto it = std::find_if(buffers_.begin(), buffers_.end(),
                         [buffer](const auto& value) { return value.buffer == buffer; });
  if (it == buffers_.end()) {
    util::msg::fatal("retaining buffer that does not exist");
  }

  ++it->ref_count;
}

void Context::release_buffer_(VkBuffer buffer) noexcept {
  auto it = std::find_if(buffers_.begin(), buffers_.end(),
                         [buffer](const auto& value) { return value.buffer == buffer; });
  if (it == buffers_.end()) {
    return;
  }

  if (--it->ref_count == 0) {
    vmaDestroyBuffer(memory_allocator_, it->buffer, it->allocation);
    buffers_.erase(it);
  }
}

}  // namespace crystal::vulkan
