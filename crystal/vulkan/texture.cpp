#include "crystal/vulkan/texture.hpp"

#include <limits>

#include "crystal/vulkan/command_buffer.hpp"
#include "crystal/vulkan/context.hpp"

// namespace {

// VkDeviceMemory allocate_memory(VkDevice device, VkPhysicalDevice physical_device, VkImage image)
// {
//   VkMemoryRequirements memory_requirements = {};
//   vkGetImageMemoryRequirements(device, image, &memory_requirements);

//   VkPhysicalDeviceMemoryProperties memory_properties = {};
//   vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

//   for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
//     if (memory_requirements.memoryTypeBits & (1 << i)) {
//       const VkMemoryAllocateInfo memory_allocate_info = {
//           /* sType = */ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
//           /* pNext           = */ nullptr,
//           /* allocationSize  = */ memory_requirements.size,
//           /* memoryTypeIndex = */ i,
//       };

//       VkDeviceMemory memory = VK_NULL_HANDLE;
//       VK_ASSERT(vkAllocateMemory(device, &memory_allocate_info, nullptr, &memory),
//                 "allocating image memory");

//       return memory;
//     }
//   }

//   util::msg::fatal("allocating image memory");
// }

// }  // namespace

namespace crystal::vulkan {

Texture::Texture(Texture&& other)
    : device_(other.device_),
      memory_allocator_(other.memory_allocator_),
      allocation_(other.allocation_),
      image_(other.image_),
      image_view_(other.image_view_),
      sampler_(other.sampler_),
      format_(other.format_),
      layout_(other.layout_),
      extent_(other.extent_) {
  other.device_           = VK_NULL_HANDLE;
  other.memory_allocator_ = VK_NULL_HANDLE;
  other.allocation_       = VK_NULL_HANDLE;
  other.image_            = VK_NULL_HANDLE;
  other.image_view_       = VK_NULL_HANDLE;
  other.sampler_          = VK_NULL_HANDLE;
  other.format_           = VK_FORMAT_UNDEFINED;
  other.layout_           = VK_IMAGE_LAYOUT_UNDEFINED;
  other.extent_           = {};
}

Texture& Texture::operator=(Texture&& other) {
  // TODO: destroy the texture.

  device_           = other.device_;
  memory_allocator_ = other.memory_allocator_;
  allocation_       = other.allocation_;
  image_            = other.image_;
  image_view_       = other.image_view_;
  sampler_          = other.sampler_;
  format_           = other.format_;
  layout_           = other.layout_;
  extent_           = other.extent_;

  other.device_           = VK_NULL_HANDLE;
  other.memory_allocator_ = VK_NULL_HANDLE;
  other.allocation_       = VK_NULL_HANDLE;
  other.image_            = VK_NULL_HANDLE;
  other.image_view_       = VK_NULL_HANDLE;
  other.sampler_          = VK_NULL_HANDLE;
  other.format_           = VK_FORMAT_UNDEFINED;
  other.layout_           = VK_IMAGE_LAYOUT_UNDEFINED;
  other.extent_           = {};

  return *this;
}

Texture::~Texture() {
  if (device_ == VK_NULL_HANDLE) {
    return;
  }

  // TODO: destroy the texture.
}

Texture::Texture(Context& ctx, const TextureDesc& desc) {
  device_           = ctx.device_;
  memory_allocator_ = ctx.memory_allocator_;

  switch (desc.format) {
    case TextureFormat::Undefined:
      util::msg::fatal("undefined texture format");

    case TextureFormat::RGBA32u:
      format_ = VK_FORMAT_R8G8B8A8_UNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RGBA32s:
      format_ = VK_FORMAT_R8G8B8A8_SNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::Depth32f:
      format_ = VK_FORMAT_D32_SFLOAT;
      layout_ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      break;
  }

  extent_ = {
      desc.width,
      desc.height,
  };

  // {  // Create the image.
  //   const VkImageCreateInfo create_info = {
  //       /* .sType = */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
  //       /* .pNext                 = */ nullptr,
  //       /* .flags                 = */ 0,
  //       /* .imageType             = */ VK_IMAGE_TYPE_2D,
  //       /* .format                = */ format_,
  //       /* .extent                = */
  //       {
  //           desc.width,
  //           desc.height,
  //           1,
  //       },
  //       /* .mipLevels             = */ 1,
  //       /* .arrayLayers           = */ 1,
  //       /* .samples               = */ VK_SAMPLE_COUNT_1_BIT,
  //       /* .tiling                = */ VK_IMAGE_TILING_OPTIMAL,
  //       /* .usage                 = */
  //       static_cast<VkImageUsageFlags>(
  //           VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
  //           (layout_ == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  //                ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
  //                : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
  //           (desc.sample != TextureSample::None ? VK_IMAGE_USAGE_SAMPLED_BIT : 0)),
  //       /* .sharingMode           = */ VK_SHARING_MODE_EXCLUSIVE,
  //       /* .queueFamilyIndexCount = */ 0,
  //       /* .pQueueFamilyIndices   = */ nullptr,
  //       /* .initialLayout         = */ VK_IMAGE_LAYOUT_UNDEFINED,
  //   };

  //   VK_ASSERT(vkCreateImage(device_, &create_info, nullptr, &image_), "creating image");
  // }

  // {  // Allocate device memory.
  //   memory_ = allocate_memory(device_, ctx.physical_device_, image_);
  //   VK_ASSERT(vkBindImageMemory(device_, image_, memory_, 0), "binding memory to image");
  // }

  {  // Creating image.
    const VkImageCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        /* .pNext                 = */ nullptr,
        /* .flags                 = */ 0,
        /* .imageType             = */ VK_IMAGE_TYPE_2D,
        /* .format                = */ format_,
        /* .extent                = */
        {
            desc.width,
            desc.height,
            1,
        },
        /* .mipLevels             = */ 1,
        /* .arrayLayers           = */ 1,
        /* .samples               = */ VK_SAMPLE_COUNT_1_BIT,
        /* .tiling                = */ VK_IMAGE_TILING_OPTIMAL,
        /* .usage                 = */
        static_cast<VkImageUsageFlags>(
            (layout_ == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                 ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                 : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
            (desc.sample != TextureSample::None ? VK_IMAGE_USAGE_SAMPLED_BIT : 0)),
        /* .sharingMode           = */ VK_SHARING_MODE_EXCLUSIVE,
        /* .queueFamilyIndexCount = */ 0,
        /* .pQueueFamilyIndices   = */ nullptr,
        /* .initialLayout         = */ VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VmaAllocationCreateInfo alloc_info = {
        /* .flags          = */ 0,
        /* .usage          = */ VMA_MEMORY_USAGE_GPU_ONLY,
        /* .requiredFlags  = */ 0,
        /* .preferredFlags = */ 0,
        /* .memoryTypeBits = */ 0,
        /* .pool           = */ VK_NULL_HANDLE,
        /* .pUserData      = */ nullptr,
    };

    VK_ASSERT(vmaCreateImage(ctx.memory_allocator_, &create_info, &alloc_info, &image_,
                             &allocation_, nullptr),
              "creating image");
  }

  {  // Create image view.
    const VkImageViewCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        /* .pNext            = */ nullptr,
        /* .flags            = */ 0,
        /* .image            = */ image_,
        /* .viewType         = */ VK_IMAGE_VIEW_TYPE_2D,
        /* .format           = */ format_,
        /* .components       = */
        {
            /* .r = */ VK_COMPONENT_SWIZZLE_IDENTITY,
            /* .g = */ VK_COMPONENT_SWIZZLE_IDENTITY,
            /* .b = */ VK_COMPONENT_SWIZZLE_IDENTITY,
            /* .a = */ VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        /* .subresourceRange = */
        {
            /* .aspectMask     = */ (layout_ == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                         ? VK_IMAGE_ASPECT_DEPTH_BIT
                                         : VK_IMAGE_ASPECT_COLOR_BIT),
            /* .baseMipLevel   = */ 0,
            /* .levelCount     = */ 1,
            /* .baseArrayLayer = */ 0,
            /* .layerCount     = */ 1,
        },
    };

    VK_ASSERT(vkCreateImageView(device_, &create_info, nullptr, &image_view_),
              "creating image view");
  }

  if (desc.sample != TextureSample::None) {  // Create sampler.
    const VkSamplerCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        /* .pNext                   = */ nullptr,
        /* .flags                   = */ 0,
        /* .magFilter               = */ VK_FILTER_LINEAR,
        /* .minFilter               = */ VK_FILTER_LINEAR,
        /* .mipmapMode              = */ VK_SAMPLER_MIPMAP_MODE_NEAREST,
        /* .addressModeU            = */
        (uint32_t(desc.repeat) & uint32_t(TextureRepeat::RepeatX))
            ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
            : VK_SAMPLER_ADDRESS_MODE_REPEAT,
        /* .addressModeV            = */
        (uint32_t(desc.repeat) & uint32_t(TextureRepeat::RepeatY))
            ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
            : VK_SAMPLER_ADDRESS_MODE_REPEAT,
        /* .addressModeW            = */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        /* .mipLodBias              = */ 0,
        /* .anisotropyEnable        = */ false,
        /* .maxAnisotropy           = */ 0.0f,
        /* .compareEnable           = */ true,
        /* .compareOp               = */ VK_COMPARE_OP_LESS,
        /* .minLod                  = */ 0.0f,
        /* .maxLod                  = */ 1.0f,
        /* .borderColor             = */ VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        /* .unnormalizedCoordinates = */ false,
    };

    VK_ASSERT(vkCreateSampler(device_, &create_info, nullptr, &sampler_), "creating sampler");
  }
}

}  // namespace crystal::vulkan
