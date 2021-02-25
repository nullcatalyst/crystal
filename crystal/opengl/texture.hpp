#pragma once

#include <cstdint>

#include "crystal/common/texture_desc.hpp"
#include "crystal/opengl/gl.hpp"

namespace crystal::opengl {

class Context;
class RenderPass;

class Texture {
  Context* ctx_     = nullptr;
  GLuint   texture_ = 0;
  uint32_t width_   = 0;
  uint32_t height_  = 0;

public:
  constexpr Texture() = default;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other);
  Texture& operator=(Texture&& other);

  ~Texture();

  void destroy() noexcept;

private:
  friend class ::crystal::opengl::Context;
  friend class ::crystal::opengl::RenderPass;

  Texture(Context& ctx, const TextureDesc& desc);
};

}  // namespace crystal::opengl
