#include "crystal/metal/texture.hpp"

#include "crystal/metal/context.hpp"
#include "util/msg/msg.hpp"

namespace crystal::metal {

Texture::Texture(Texture&& other) : texture_(other.texture_), pixel_format_(other.pixel_format_) {
  other.texture_      = nullptr;
  other.pixel_format_ = static_cast<MTLPixelFormat>(0);
}

Texture& Texture::operator=(Texture&& other) {
  destroy();

  texture_      = other.texture_;
  pixel_format_ = other.pixel_format_;

  other.texture_      = nullptr;
  other.pixel_format_ = static_cast<MTLPixelFormat>(0);

  return *this;
}

Texture::~Texture() { destroy(); }

void Texture::destroy() noexcept {
  texture_      = nullptr;
  pixel_format_ = static_cast<MTLPixelFormat>(0);
}

Texture::Texture(OBJC(MTLDevice) device, const TextureDesc& desc) {
  MTLTextureDescriptor* texture_desc = [[MTLTextureDescriptor alloc] init];
  texture_desc.width                 = desc.width;
  texture_desc.height                = desc.height;
  texture_desc.textureType           = MTLTextureType2D;
  texture_desc.sampleCount           = 1;

  switch (desc.format) {
    case TextureFormat::R8u:
      pixel_format_ = MTLPixelFormatR8Unorm;
      break;

    case TextureFormat::RG8u:
      pixel_format_ = MTLPixelFormatRG8Unorm;
      break;

    case TextureFormat::RGB8u:
      // Metal doesn't support a 3-component "MTLPixelFormatRGB8Unorm",
      // use 4-component "MTLPixelFormatRGBA8Unorm" instead
      pixel_format_ = MTLPixelFormatRGBA8Unorm;
      break;

    case TextureFormat::RGBA8u:
      pixel_format_ = MTLPixelFormatRGBA8Unorm;
      break;

    case TextureFormat::R8s:
      pixel_format_ = MTLPixelFormatR8Snorm;
      break;

    case TextureFormat::RG8s:
      pixel_format_ = MTLPixelFormatRG8Snorm;
      break;

    case TextureFormat::RGB8s:
      // Metal doesn't support a 3-component "MTLPixelFormatRGB8Snorm",
      // use 4-component "MTLPixelFormatRGBA8Snorm" instead
      pixel_format_ = MTLPixelFormatRGBA8Snorm;
      break;

    case TextureFormat::RGBA8s:
      pixel_format_ = MTLPixelFormatRGBA8Snorm;
      break;

    case TextureFormat::Depth32f:
      pixel_format_ = MTLPixelFormatDepth32Float;
      break;

    default:
      util::msg::fatal("creating texture with unsupported format [",
                       static_cast<size_t>(desc.format), "]");
  }

  texture_desc.pixelFormat = pixel_format_;
  texture_                 = [device newTextureWithDescriptor:texture_desc];
}

}  // namespace crystal::metal
