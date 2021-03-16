#include "crystal/metal/pipeline.hpp"

#include <algorithm>  // for_each
#include "crystal/metal/context.hpp"
#include "crystal/metal/library.hpp"
#include "util/fs/file.hpp"
#include "util/fs/path.hpp"

namespace crystal::metal {

Pipeline::Pipeline(Pipeline&& other)
    : render_pipeline_(other.render_pipeline_),
      depth_stencil_state_(other.depth_stencil_state_),
      cull_mode_(other.cull_mode_) {
  other.render_pipeline_     = nullptr;
  other.depth_stencil_state_ = nullptr;
  other.cull_mode_           = MTLCullModeNone;
}

Pipeline& Pipeline::operator=(Pipeline&& other) {
  destroy();

  render_pipeline_     = other.render_pipeline_;
  depth_stencil_state_ = other.depth_stencil_state_;
  cull_mode_           = other.cull_mode_;

  other.render_pipeline_     = nullptr;
  other.depth_stencil_state_ = nullptr;
  other.cull_mode_           = MTLCullModeNone;

  return *this;
}

Pipeline::~Pipeline() { destroy(); }

void Pipeline::destroy() noexcept {
  render_pipeline_     = nullptr;
  depth_stencil_state_ = nullptr;
  cull_mode_           = MTLCullModeNone;
}

Pipeline::Pipeline(OBJC(MTLDevice) device, Library& library, RenderPass& render_pass,
                   const PipelineDesc& desc) {
  MTLVertexDescriptor* vertex_descriptor = [[MTLVertexDescriptor alloc] init];

  std::for_each(desc.vertex_attributes.begin(), desc.vertex_attributes.end(),
                [vertex_descriptor](auto vertex_attribute) {
                  vertex_descriptor.attributes[vertex_attribute.id].format = MTLVertexFormatFloat4;
                  vertex_descriptor.attributes[vertex_attribute.id].offset =
                      vertex_attribute.offset;
                  vertex_descriptor.attributes[vertex_attribute.id].bufferIndex =
                      vertex_attribute.buffer_index;
                });

  std::for_each(desc.vertex_buffers.begin(), desc.vertex_buffers.end(),
                [vertex_descriptor](auto vertex_buffer) {
                  vertex_descriptor.layouts[vertex_buffer.buffer_index].stride =
                      vertex_buffer.stride;
                  vertex_descriptor.layouts[vertex_buffer.buffer_index].stepRate = 1;
                  vertex_descriptor.layouts[vertex_buffer.buffer_index].stepFunction =
                      vertex_buffer.step_function == StepFunction::PerVertex
                          ? MTLVertexStepFunctionPerVertex
                          : MTLVertexStepFunctionPerInstance;
                });

  std::string vertex_name;
  std::string fragment_name;
  const auto& metal_pb = library.lib_pb_.metal();
  std::cout << "metal_pipelines=" << metal_pb.pipelines_size() << std::endl;
  for (int i = 0; i < metal_pb.pipelines_size(); ++i) {
    const auto& pipeline_pb = metal_pb.pipelines(i);
    std::cout << "pipeline_name=" << pipeline_pb.name() << std::endl;
    if (pipeline_pb.name() != desc.name) {
      continue;
    }

    vertex_name   = pipeline_pb.vertex_name();
    fragment_name = pipeline_pb.fragment_name();

    std::cout << "vertex_name=" << vertex_name << std::endl;
    std::cout << "fragment_name=" << fragment_name << std::endl;

    // Initialize the uniforms.
    // uniforms_ = {};
    // for (const auto& uniform_pb : pipeline_pb.uniforms()) {
    //   uniforms_[uniform_pb.binding()] = glGetUniformBlockIndex(program_,
    //   uniform_pb.name().c_str());
    // }

    break;
  }

  MTLRenderPipelineDescriptor* pipeline_state_desc = [[MTLRenderPipelineDescriptor alloc] init];
  id<MTLFunction>              vertex_function =
      [library.library_ newFunctionWithName:[NSString stringWithCString:vertex_name.c_str()
                                                               encoding:NSUTF8StringEncoding]];
  id<MTLFunction> fragment_function = nullptr;

  if (fragment_name.size() > 0) {
    fragment_function =
        [library.library_ newFunctionWithName:[NSString stringWithCString:fragment_name.c_str()
                                                                 encoding:NSUTF8StringEncoding]];
  }

  [pipeline_state_desc setVertexDescriptor:vertex_descriptor];
  [pipeline_state_desc setVertexFunction:vertex_function];
  [pipeline_state_desc setFragmentFunction:fragment_function];

  for (int i = 0; i < render_pass.color_count_; ++i) {
    pipeline_state_desc.colorAttachments[i].pixelFormat = render_pass.color_pixel_formats_[i];
  }
  if (render_pass.has_depth_) {
    pipeline_state_desc.depthAttachmentPixelFormat = render_pass.depth_pixel_format_;
  }

  MTLDepthStencilDescriptor* depth_stencil_desc = [[MTLDepthStencilDescriptor alloc] init];
  depth_stencil_desc.depthCompareFunction       = static_cast<MTLCompareFunction>(desc.depth_test);
  depth_stencil_desc.depthWriteEnabled          = desc.depth_write == DepthWrite::Enable;
  depth_stencil_state_ = [device newDepthStencilStateWithDescriptor:depth_stencil_desc];

  switch (desc.cull_mode) {
    case CullMode::None:
      cull_mode_ = MTLCullModeNone;
      break;
    case CullMode::Front:
      cull_mode_ = MTLCullModeFront;
      break;
    case CullMode::Back:
      cull_mode_ = MTLCullModeBack;
      break;
    default:
      cull_mode_ = MTLCullModeNone;
      break;
  }

  NSError* error   = nullptr;
  render_pipeline_ = [device newRenderPipelineStateWithDescriptor:pipeline_state_desc error:&error];
  if (error != nullptr) {
    const char* error_message = [[error localizedDescription] UTF8String];
    util::msg::fatal("creating render pipeline state: ", error_message);
  }
}

}  // namespace crystal::metal
