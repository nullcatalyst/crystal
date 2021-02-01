#pragma once

#include <cstdint>

#include "crystal/metal/mtl.hpp"

namespace crystal::metal {

class Context;
class Mesh;
class Pipeline;
class RenderPass;
class UniformBuffer;

class CommandBuffer {
  OBJC(CAMetalDrawable) metal_drawable_         = nullptr;
  OBJC(MTLCommandBuffer) command_buffer_        = nullptr;
  OBJC(MTLRenderCommandEncoder) render_encoder_ = nullptr;

public:
  CommandBuffer(const CommandBuffer&) = delete;
  CommandBuffer& operator=(const CommandBuffer&) = delete;

  CommandBuffer(CommandBuffer&&) = delete;
  CommandBuffer& operator=(CommandBuffer&&) = delete;

  ~CommandBuffer();

  void use_render_pass(RenderPass& render_pass);
  void use_pipeline(Pipeline& pipeline);
  void use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t binding);
  void use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t location, uint32_t binding);

  void draw(Mesh& mesh, uint32_t vertex_count, uint32_t instance_count);

private:
  friend class ::crystal::metal::Context;
  friend class ::crystal::metal::RenderPass;

  CommandBuffer(OBJC(CAMetalDrawable) metal_drawable, OBJC(MTLCommandBuffer) command_buffer);
};

}  // namespace crystal::metal
