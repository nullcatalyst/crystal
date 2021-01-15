#pragma once

#include "crystal/opengl/gl.hpp"

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES
#define CRYSTAL_IMPL_CTOR Texture(Context& ctx, const TextureDesc& desc);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/texture.inl"
