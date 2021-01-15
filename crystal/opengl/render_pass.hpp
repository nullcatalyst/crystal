#pragma once

#include "crystal/common/render_pass_desc.hpp"
#include "crystal/opengl/gl.hpp"

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES \
  Context* ctx_;                \
  GLuint   framebuffer_;        \
  uint32_t width_;              \
  uint32_t height_;
#define CRYSTAL_IMPL_CTOR   \
  RenderPass(Context& ctx); \
  RenderPass(Context& ctx, const RenderPassDesc& desc);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/render_pass.inl"
