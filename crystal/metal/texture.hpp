#pragma once

#include <cstdint>

#include "crystal/common/texture_desc.hpp"
#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class CommandBuffer;
class RenderPass;

class Texture {
  OBJC(MTLTexture) texture_    = nullptr;
  MTLPixelFormat pixel_format_ = static_cast<MTLPixelFormat>(0);

public:
  constexpr Texture() = default;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other);
  Texture& operator=(Texture&& other);

  ~Texture();

  void destroy() noexcept;

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::CommandBuffer;
  friend class ::crystal::metal::RenderPass;

  Texture(OBJC(MTLDevice) device, const TextureDesc& desc);
};

}  // namespace crystal::metal
