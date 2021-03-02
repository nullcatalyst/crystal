#include "crystal/metal/command_buffer.hpp"

#include <algorithm>  // find_if

#include "crystal/metal/mesh.hpp"
#include "crystal/metal/pipeline.hpp"
#include "crystal/metal/render_pass.hpp"
#include "crystal/metal/uniform_buffer.hpp"

namespace crystal::metal {

CommandBuffer::~CommandBuffer() {
  if (render_encoder_ != nullptr) {
    [render_encoder_ endEncoding];
  }

  [command_buffer_ presentDrawable:metal_drawable_];
  [command_buffer_ commit];
}

void CommandBuffer::use_render_pass(RenderPass& render_pass) {
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
                                   /* .znear   = */ 1.0,
                                   /* .zfar    = */ 0.0,
                               }];
  // [render_encoder_ setScissorRect:MTLScissorRect{
  //                                     /* .x      = */ 0,
  //                                     /* .y      = */ 0,
  //                                     /* .width  = */ render_pass.width_,
  //                                     /* .height = */ render_pass.height_,
  //                                 }];
}

void CommandBuffer::use_pipeline(Pipeline& pipeline) {
  [render_encoder_ setRenderPipelineState:pipeline.render_pipeline_];
  [render_encoder_ setDepthStencilState:pipeline.depth_stencil_state_];
  [render_encoder_ setCullMode:pipeline.cull_mode_];
}

void CommandBuffer::use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t location,
                                       uint32_t binding) {
  [render_encoder_ setVertexBuffer:uniform_buffer.buffer_ offset:0 atIndex:binding];
  [render_encoder_ setFragmentBuffer:uniform_buffer.buffer_ offset:0 atIndex:binding];
}

void CommandBuffer::draw(Mesh& mesh, uint32_t vertex_or_index_count, uint32_t instance_count) {
  for (int i = 0; i < mesh.binding_count_; ++i) {
    [render_encoder_ setVertexBuffer:mesh.bindings_[i].buffer
                              offset:0
                             atIndex:mesh.bindings_[i].index];
  }

  [render_encoder_ drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:vertex_or_index_count];
}

CommandBuffer::CommandBuffer(OBJC(CAMetalDrawable) metal_drawable,
                             OBJC(MTLCommandBuffer) command_buffer)
    : metal_drawable_(metal_drawable), command_buffer_(command_buffer) {}

}  // namespace crystal::metal
