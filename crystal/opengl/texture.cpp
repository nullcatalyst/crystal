#include "crystal/opengl/texture.hpp"

#include "crystal/opengl/context.hpp"

namespace crystal::opengl {

Texture::Texture(Texture&& other)
    : ctx_(other.ctx_), texture_(other.texture_), width_(other.width_), height_(other.height_) {
  other.ctx_     = nullptr;
  other.texture_ = 0;
  other.width_   = 0;
  other.height_  = 0;
}

Texture& Texture::operator=(Texture&& other) {
  destroy();

  ctx_     = other.ctx_;
  texture_ = other.texture_;
  width_   = other.width_;
  height_  = other.height_;

  other.ctx_     = nullptr;
  other.texture_ = 0;
  other.width_   = 0;
  other.height_  = 0;

  return *this;
}

Texture::~Texture() { destroy(); }

void Texture::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  ctx_->release_texture_(texture_);

  ctx_     = nullptr;
  texture_ = 0;
  width_   = 0;
  height_  = 0;
}

Texture::Texture(Context& ctx, const TextureDesc& desc)
    : ctx_(&ctx), texture_(0), width_(desc.width), height_(desc.height) {
  GL_ASSERT(glGenTextures(1, &texture_), "generating texture");
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, texture_), "binding texture");

  GLenum internal_format = 0;
  GLenum format          = 0;
  GLenum type            = 0;
  switch (desc.format) {
    case TextureFormat::RGBA32u:
      internal_format = GL_RGBA8;
      format          = GL_RGBA;
      type            = GL_UNSIGNED_BYTE;
      break;

    case TextureFormat::RGBA32s:
      internal_format = GL_RGBA8_SNORM;
      format          = GL_RGBA;
      type            = GL_BYTE;
      break;

    case TextureFormat::Depth32f:
      internal_format = GL_DEPTH_COMPONENT32F;
      format          = GL_DEPTH_COMPONENT;
      type            = GL_FLOAT;
      break;

    default:
      util::msg::fatal("creating texture with unsupported format [",
                       static_cast<size_t>(desc.format), "]");
  }

  GL_ASSERT(
      glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width_, height_, 0, format, type, nullptr),
      "reserving texture memory");

  switch (desc.sample) {
    case TextureSample::Nearest:
      GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST),
                "setting texture min filter");
      GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST),
                "setting texture mag filter");
      break;

    case TextureSample::Linear:
      GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR),
                "setting texture min filter");
      GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR),
                "setting texture mag filter");
      break;

    default:
      util::msg::fatal("creating texture with unsupported sample [",
                       static_cast<size_t>(desc.sample), "]");
  }

  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE), "setting wrap s");
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE), "setting wrap t");

  ctx_->add_texture_(texture_);
}

}  // namespace crystal::opengl
