#include "crystal/vulkan/texture.hpp"

#include <limits>

#include "crystal/vulkan/command_buffer.hpp"
#include "crystal/vulkan/context.hpp"

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
  destroy();

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

Texture::~Texture() { destroy(); }

void Texture::destroy() {
  if (device_ == VK_NULL_HANDLE) {
    return;
  }

  vkDestroySampler(device_, sampler_, nullptr);
  vkDestroyImageView(device_, image_view_, nullptr);
  vmaDestroyImage(memory_allocator_, image_, allocation_);

  device_           = VK_NULL_HANDLE;
  memory_allocator_ = VK_NULL_HANDLE;
  allocation_       = VK_NULL_HANDLE;
  image_            = VK_NULL_HANDLE;
  image_view_       = VK_NULL_HANDLE;
  sampler_          = VK_NULL_HANDLE;
  format_           = VK_FORMAT_UNDEFINED;
  layout_           = VK_IMAGE_LAYOUT_UNDEFINED;
  extent_           = {};
}

Texture::Texture(Context& ctx, const TextureDesc& desc)
    : device_(ctx.device_), memory_allocator_(ctx.memory_allocator_) {
  bool depth = false;

  switch (desc.format) {
    case TextureFormat::R8u:
      format_ = VK_FORMAT_R8_UNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RG8u:
      format_ = VK_FORMAT_R8G8_UNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RGB8u:
      format_ = VK_FORMAT_R8G8B8_UNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RGBA8u:
      format_ = VK_FORMAT_R8G8B8A8_UNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::R8s:
      format_ = VK_FORMAT_R8_SNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RG8s:
      format_ = VK_FORMAT_R8G8_SNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RGB8s:
      format_ = VK_FORMAT_R8G8B8_SNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::RGBA8s:
      format_ = VK_FORMAT_R8G8B8A8_SNORM;
      layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      break;

    case TextureFormat::Depth32f:
      depth   = true;
      format_ = VK_FORMAT_D32_SFLOAT;
      layout_ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      break;

    default:
      util::msg::fatal("unsupported texture format [", static_cast<uint32_t>(desc.format), "]");
  }

  extent_ = {
      desc.width,
      desc.height,
  };

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
        static_cast<VkImageUsageFlags>((depth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                                              : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
                                       VK_IMAGE_USAGE_SAMPLED_BIT),
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
            /* .aspectMask     = */ static_cast<VkImageAspectFlags>(
                depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
            /* .baseMipLevel   = */ 0,
            /* .levelCount     = */ 1,
            /* .baseArrayLayer = */ 0,
            /* .layerCount     = */ 1,
        },
    };

    VK_ASSERT(vkCreateImageView(device_, &create_info, nullptr, &image_view_),
              "creating image view");
  }

  {  // Create sampler.
    VkFilter filter;
    switch (desc.sample) {
      case TextureSample::Nearest:
        filter = VK_FILTER_NEAREST;
        break;

      case TextureSample::Linear:
        filter = VK_FILTER_LINEAR;
        break;

      default:
        util::msg::fatal("unknown texture sample mode [", static_cast<int>(desc.sample), "]");
    }

    const VkSamplerCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        /* .pNext                   = */ nullptr,
        /* .flags                   = */ 0,
        /* .magFilter               = */ filter,
        /* .minFilter               = */ filter,
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
        /* .compareEnable           = */ false,
        /* .compareOp               = */ VK_COMPARE_OP_LESS,
        /* .minLod                  = */ 0.0f,
        /* .maxLod                  = */ 1.0f,
        /* .borderColor             = */
        depth ? VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE : VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        /* .unnormalizedCoordinates = */ false,
    };

    VK_ASSERT(vkCreateSampler(device_, &create_info, nullptr, &sampler_), "creating sampler");
  }
}

}  // namespace crystal::vulkan
