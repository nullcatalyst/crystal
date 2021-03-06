#include "crystal/metal/command_buffer.hpp"

#include <algorithm>  // find_if

#include "crystal/metal/mesh.hpp"
#include "crystal/metal/pipeline.hpp"
#include "crystal/metal/render_pass.hpp"
#include "crystal/metal/texture.hpp"
#include "crystal/metal/uniform_buffer.hpp"
#include "util/msg/msg.hpp"

namespace crystal::metal {

CommandBuffer::~CommandBuffer() {
  if (render_encoder_ != nullptr) {
    [render_encoder_ endEncoding];
  }

  [command_buffer_ presentDrawable:metal_drawable_];
  [command_buffer_ commit];
}

void CommandBuffer::use_render_pass(const RenderPass& render_pass) {
  if (render_encoder_ != nullptr) {
    [render_encoder_ endEncoding];
  }

  render_encoder_ =
      [command_buffer_ renderCommandEncoderWithDescriptor:render_pass.render_pass_desc_];
  [render_encoder_ setViewport:MTLViewport{
                                   /* .originX = */ 0.0,
                                   /* .originY = */ 0.0,
                                   /* .width   = */ static_cast<double>(render_pass.width_),
                                   /* .height  = */ static_cast<double>(render_pass.height_),
                                   /* .znear   = */ 0.0,
                                   /* .zfar    = */ 1.0,
                               }];
  [render_encoder_ setScissorRect:MTLScissorRect{
                                      /* .x      = */ 0,
                                      /* .y      = */ 0,
                                      /* .width  = */ render_pass.width_,
                                      /* .height = */ render_pass.height_,
                                  }];
}

void CommandBuffer::use_pipeline(const Pipeline& pipeline) {
  pipeline_ = &pipeline;

  [render_encoder_ setRenderPipelineState:pipeline.render_pipeline_];
  [render_encoder_ setDepthStencilState:pipeline.depth_stencil_state_];
  [render_encoder_ setCullMode:pipeline.cull_mode_];
  [render_encoder_ setFrontFacingWinding:pipeline.winding_];
}

void CommandBuffer::use_uniform_buffer(const UniformBuffer& uniform_buffer, uint32_t binding) {
  if (pipeline_ == nullptr) {
    util::msg::fatal("using uniform buffer at binding [", binding, "] without a pipeline bound");
  }

  const auto index = pipeline_->uniforms_[binding];
  [render_encoder_ setVertexBuffer:uniform_buffer.buffer_ offset:0 atIndex:index];
  [render_encoder_ setFragmentBuffer:uniform_buffer.buffer_ offset:0 atIndex:index];
}

void CommandBuffer::use_texture(const Texture& texture, uint32_t binding) {
  [render_encoder_ setFragmentTexture:texture.texture_ atIndex:binding];
  [render_encoder_ setFragmentSamplerState:texture.sampler_ atIndex:binding];
}

void CommandBuffer::draw(const Mesh& mesh, uint32_t vertex_or_index_count,
                         uint32_t instance_count) {
  for (int i = 0; i < mesh.binding_count_; ++i) {
    [render_encoder_ setVertexBuffer:mesh.bindings_[i].buffer
                              offset:0
                             atIndex:mesh.bindings_[i].index];
  }

  if (mesh.index_buffer_ != nullptr) {
    [render_encoder_ drawIndexedPrimitives:MTLPrimitiveTypeTriangleStrip
                                indexCount:vertex_or_index_count
                                 indexType:MTLIndexTypeUInt16
                               indexBuffer:mesh.index_buffer_
                         indexBufferOffset:0
                             instanceCount:instance_count];
  } else {
    [render_encoder_ drawPrimitives:MTLPrimitiveTypeTriangleStrip
                        vertexStart:0
                        vertexCount:vertex_or_index_count
                      instanceCount:instance_count];
  }
}

CommandBuffer::CommandBuffer(OBJC(CAMetalDrawable) metal_drawable,
                             OBJC(MTLCommandBuffer) command_buffer)
    : metal_drawable_(metal_drawable), command_buffer_(command_buffer) {}

}  // namespace crystal::metal
