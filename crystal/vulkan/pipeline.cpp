#include "crystal/vulkan/pipeline.hpp"

#include "crystal/common/proto/proto.hpp"
#include "crystal/vulkan/context.hpp"
#include "crystal/vulkan/library.hpp"
#include "crystal/vulkan/render_pass.hpp"

namespace crystal::vulkan {

Pipeline::Pipeline(Pipeline&& other)
    : device_(other.device_),
      descriptor_pool_(other.descriptor_pool_),
      render_pass_(other.render_pass_),
      uniform_descriptor_set_layout_(other.uniform_descriptor_set_layout_),
      uniform_descriptor_sets_(std::move(other.uniform_descriptor_sets_)),
      texture_descriptor_set_layout_(other.texture_descriptor_set_layout_),
      texture_descriptor_sets_(std::move(other.texture_descriptor_sets_)),
      pipeline_layout_(other.pipeline_layout_),
      pipeline_(other.pipeline_) {
  other.device_                        = VK_NULL_HANDLE;
  other.descriptor_pool_               = VK_NULL_HANDLE;
  other.render_pass_                   = VK_NULL_HANDLE;
  other.uniform_descriptor_set_layout_ = VK_NULL_HANDLE;
  other.texture_descriptor_set_layout_ = VK_NULL_HANDLE;
  other.pipeline_layout_               = VK_NULL_HANDLE;
  other.pipeline_                      = VK_NULL_HANDLE;
}

Pipeline& Pipeline::operator=(Pipeline&& other) {
  destroy();

  device_                        = other.device_;
  descriptor_pool_               = other.descriptor_pool_;
  render_pass_                   = other.render_pass_;
  uniform_descriptor_set_layout_ = other.uniform_descriptor_set_layout_;
  uniform_descriptor_sets_       = std::move(other.uniform_descriptor_sets_);
  texture_descriptor_set_layout_ = other.texture_descriptor_set_layout_;
  texture_descriptor_sets_       = std::move(other.texture_descriptor_sets_);
  pipeline_layout_               = other.pipeline_layout_;
  pipeline_                      = other.pipeline_;

  other.device_                        = VK_NULL_HANDLE;
  other.descriptor_pool_               = VK_NULL_HANDLE;
  other.render_pass_                   = VK_NULL_HANDLE;
  other.uniform_descriptor_set_layout_ = VK_NULL_HANDLE;
  other.uniform_descriptor_sets_       = {};
  other.texture_descriptor_set_layout_ = VK_NULL_HANDLE;
  other.texture_descriptor_sets_       = {};
  other.pipeline_layout_               = VK_NULL_HANDLE;
  other.pipeline_                      = VK_NULL_HANDLE;

  return *this;
}

Pipeline::~Pipeline() { destroy(); }

void Pipeline::destroy() noexcept {
  if (device_ == VK_NULL_HANDLE) {
    return;
  }

  if (texture_descriptor_set_layout_ != VK_NULL_HANDLE) {
    VK_ASSERT(vkFreeDescriptorSets(device_, descriptor_pool_, texture_descriptor_sets_.size(),
                                   texture_descriptor_sets_.data()),
              "freeing descriptor sets");
    vkDestroyDescriptorSetLayout(device_, texture_descriptor_set_layout_, nullptr);
  }
  if (uniform_descriptor_set_layout_ != VK_NULL_HANDLE) {
    VK_ASSERT(vkFreeDescriptorSets(device_, descriptor_pool_, uniform_descriptor_sets_.size(),
                                   uniform_descriptor_sets_.data()),
              "freeing descriptor sets");
    vkDestroyDescriptorSetLayout(device_, uniform_descriptor_set_layout_, nullptr);
  }
  vkDestroyPipeline(device_, pipeline_, nullptr);
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);

  device_                        = VK_NULL_HANDLE;
  descriptor_pool_               = VK_NULL_HANDLE;
  render_pass_                   = VK_NULL_HANDLE;
  uniform_descriptor_set_layout_ = VK_NULL_HANDLE;
  texture_descriptor_set_layout_ = VK_NULL_HANDLE;
  pipeline_layout_               = VK_NULL_HANDLE;
  pipeline_                      = VK_NULL_HANDLE;
}

Pipeline::Pipeline(Context& ctx, Library& library, RenderPass& render_pass,
                   const PipelineDesc& desc)
    : device_(ctx.device_),
      descriptor_pool_(ctx.descriptor_pool_),
      render_pass_(render_pass.render_pass_),
      uniform_descriptor_set_layout_(VK_NULL_HANDLE),
      texture_descriptor_set_layout_(VK_NULL_HANDLE) {
  const crystal::common::proto::VKPipeline* pipeline_pb = nullptr;
  const auto&                               vulkan_pb   = library.lib_pb_.vulkan();
  for (int i = 0; i < vulkan_pb.pipelines_size(); ++i) {
    const auto& check_pipeline_pb = vulkan_pb.pipelines(i);
    if (check_pipeline_pb.name() != desc.name) {
      continue;
    }

    pipeline_pb = &check_pipeline_pb;
    break;
  }

  if (pipeline_pb == nullptr) {
    util::msg::fatal("could not find pipeline [", desc.name, "]");
  }

  if (pipeline_pb->uniforms_size() > 0) {
    {  // Create uniform descriptor set layout.
      std::vector<VkDescriptorSetLayoutBinding> bindings(pipeline_pb->uniforms_size());
      for (int i = 0; i < pipeline_pb->uniforms_size(); ++i) {
        const auto& uniform_pb = pipeline_pb->uniforms(i);
        bindings[i]            = VkDescriptorSetLayoutBinding{
            /* binding            = */ uniform_pb.binding(),
            /* descriptorType     = */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /* descriptorCount    = */ 1,
            /* stageFlags         = */ VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            /* pImmutableSamplers = */ nullptr,
        };
      }

      const VkDescriptorSetLayoutCreateInfo create_info = {
          /* sType = */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          /* pNext        = */ nullptr,
          /* flags        = */ 0,
          /* bindingCount = */ static_cast<uint32_t>(bindings.size()),
          /* pBindings    = */ bindings.data(),
      };

      VK_ASSERT(vkCreateDescriptorSetLayout(device_, &create_info, nullptr,
                                            &uniform_descriptor_set_layout_),
                "creating uniform descriptor set layout");
    }

    {  // Allocate uniform descriptor sets.
      const std::array<VkDescriptorSetLayout, 4> descriptor_set_layouts{
          uniform_descriptor_set_layout_,
          uniform_descriptor_set_layout_,
          uniform_descriptor_set_layout_,
          uniform_descriptor_set_layout_,
      };
      const VkDescriptorSetAllocateInfo allocate_info = {
          /* sType = */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          /* pNext              = */ nullptr,
          /* descriptorPool     = */ ctx.descriptor_pool_,
          /* descriptorSetCount = */ static_cast<uint32_t>(descriptor_set_layouts.size()),
          /* pSetLayouts        = */ descriptor_set_layouts.data(),
      };

      VK_ASSERT(vkAllocateDescriptorSets(device_, &allocate_info, uniform_descriptor_sets_.data()),
                "allocating uniform descriptor sets");
    }
  }

  if (pipeline_pb->textures_size() > 0) {
    {  // Create texture descriptor set layout.
      std::vector<VkDescriptorSetLayoutBinding> bindings(pipeline_pb->textures_size());
      for (int i = 0; i < pipeline_pb->textures_size(); ++i) {
        const auto& texture_pb = pipeline_pb->textures(i);
        bindings[i]            = VkDescriptorSetLayoutBinding{
            /* binding            = */ texture_pb.binding(),
            /* descriptorType     = */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            /* descriptorCount    = */ 1,
            /* stageFlags         = */ VK_SHADER_STAGE_FRAGMENT_BIT,
            /* pImmutableSamplers = */ nullptr,
        };
      }

      const VkDescriptorSetLayoutCreateInfo create_info = {
          /* sType = */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          /* pNext        = */ nullptr,
          /* flags        = */ 0,
          /* bindingCount = */ static_cast<uint32_t>(bindings.size()),
          /* pBindings    = */ bindings.data(),
      };

      VK_ASSERT(vkCreateDescriptorSetLayout(device_, &create_info, nullptr,
                                            &texture_descriptor_set_layout_),
                "creating texture descriptor set layout");
    }

    {  // Allocate uniform descriptor sets.
      const std::array<VkDescriptorSetLayout, 4> descriptor_set_layouts{
          texture_descriptor_set_layout_,
          texture_descriptor_set_layout_,
          texture_descriptor_set_layout_,
          texture_descriptor_set_layout_,
      };
      const VkDescriptorSetAllocateInfo allocate_info = {
          /* sType = */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          /* pNext              = */ nullptr,
          /* descriptorPool     = */ ctx.descriptor_pool_,
          /* descriptorSetCount = */ static_cast<uint32_t>(descriptor_set_layouts.size()),
          /* pSetLayouts        = */ descriptor_set_layouts.data(),
      };

      VK_ASSERT(vkAllocateDescriptorSets(device_, &allocate_info, texture_descriptor_sets_.data()),
                "allocating texture descriptor sets");
    }
  }

  {  // Create pipeline layout.
    std::array<VkDescriptorSetLayout, 2> descriptor_set_layouts;
    uint32_t                             descriptor_set_count = 0;

    if (pipeline_pb->uniforms_size() > 0) {
      descriptor_set_layouts[descriptor_set_count] = uniform_descriptor_set_layout_;
      ++descriptor_set_count;
    }
    if (pipeline_pb->textures_size() > 0) {
      descriptor_set_layouts[descriptor_set_count] = texture_descriptor_set_layout_;
      ++descriptor_set_count;
    }

    const VkPipelineLayoutCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /* .pNext                  = */ nullptr,
        /* .flags                  = */ 0,
        /* .setLayoutCount         = */ descriptor_set_count,
        /* .pSetLayouts            = */ descriptor_set_layouts.data(),
        /* .pushConstantRangeCount = */ 0,
        /* .pPushConstantRanges    = */ nullptr,
    };

    pipeline_layout_ = VK_NULL_HANDLE;
    VK_ASSERT(vkCreatePipelineLayout(device_, &create_info, nullptr, &pipeline_layout_),
              "creating pipeline layout");
  }

  {  // Create pipeline.
    const std::string name(desc.name);

    const uint32_t shader_stage_create_info_count                     = 1 + pipeline_pb->fragment();
    const VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = {
        {
            /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /* .pNext               = */ nullptr,
            /* .flags               = */ 0,
            /* .stage               = */ VK_SHADER_STAGE_VERTEX_BIT,
            /* .module              = */ library.shader_module_,
            /* .pName               = */ name.c_str(),
            /* .pSpecializationInfo = */ nullptr,
        },
        {
            /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /* .pNext               = */ nullptr,
            /* .flags               = */ 0,
            /* .stage               = */ VK_SHADER_STAGE_FRAGMENT_BIT,
            /* .module              = */ library.shader_module_,
            /* .pName               = */ name.c_str(),
            /* .pSpecializationInfo = */ nullptr,
        },
    };

    const auto                                   desc_vertex_buffers = desc.vertex_buffers.begin();
    std::vector<VkVertexInputBindingDescription> vertex_bindings(desc.vertex_buffers.size());
    for (unsigned i = 0; i < desc.vertex_buffers.size(); ++i) {
      vertex_bindings[i] = {
          /* .binding   = */ desc_vertex_buffers[i].buffer_index,
          /* .stride    = */ desc_vertex_buffers[i].stride,
          /* .inputRate = */ static_cast<VkVertexInputRate>(desc_vertex_buffers[i].step_function),
      };
    }

    const auto desc_vertex_attributes = desc.vertex_attributes.begin();
    std::vector<VkVertexInputAttributeDescription> vertex_attributes(desc.vertex_attributes.size());
    for (unsigned i = 0; i < desc.vertex_attributes.size(); ++i) {
      vertex_attributes[i] = {
          /* .location = */ desc_vertex_attributes[i].id,
          /* .binding  = */ desc_vertex_attributes[i].buffer_index,
          /* .format   = */ VK_FORMAT_R32G32B32A32_SFLOAT,
          /* .offset   = */ desc_vertex_attributes[i].offset,
      };
    }

    const VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        /* .pNext                           = */ nullptr,
        /* .flags                           = */ 0,
        /* .vertexBindingDescriptionCount   = */ static_cast<uint32_t>(vertex_bindings.size()),
        /* .pVertexBindingDescriptions      = */ vertex_bindings.data(),
        /* .vertexAttributeDescriptionCount = */ static_cast<uint32_t>(vertex_attributes.size()),
        /* .pVertexAttributeDescriptions    = */ vertex_attributes.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        /* .pNext                  = */ nullptr,
        /* .flags                  = */ 0,
        /* .topology               = */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        /* .primitiveRestartEnable = */ VK_FALSE,
    };

    const VkPipelineViewportStateCreateInfo viewport_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        /* .pNext         = */ nullptr,
        /* .flags         = */ 0,
        /* .viewportCount = */ 1,
        /* .pViewports    = */ nullptr,
        /* .scissorCount  = */ 1,
        /* .pScissors     = */ nullptr,
    };

    const VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        /* .pNext                   = */ nullptr,
        /* .flags                   = */ 0,
        /* .depthClampEnable        = */ VK_FALSE,
        /* .rasterizerDiscardEnable = */ VK_FALSE,
        /* .polygonMode             = */ VK_POLYGON_MODE_FILL,
        /* .cullMode                = */ static_cast<VkCullModeFlags>(desc.cull_mode),
        /* .frontFace               = */ (desc.winding == Winding::Clockwise) !=
                (render_pass.framebuffers_.size() == 1)
            // Set Vulkan to the opposite of the winding direction that the caller set for crystal,
            // this is because we flipped the Vulkan y-axis (positive y-axis points up instead of
            // the Vulkan default of positive y-axis points down), using the
            // VK_KHR_MAINTENANCE1_EXTENSION extension. This flipping only applies to the screen
            // framebuffer.
            ? VK_FRONT_FACE_CLOCKWISE
            : VK_FRONT_FACE_COUNTER_CLOCKWISE,
        /* .depthBiasEnable         = */
        static_cast<VkBool32>(desc.depth_bias != 0.0f || desc.depth_slope_scale != 0.0f),
        /* .depthBiasConstantFactor = */ desc.depth_bias,
        /* .depthBiasClamp          = */ 0.0f,
        /* .depthBiasSlopeFactor    = */ desc.depth_slope_scale,
        /* .lineWidth               = */ 1.0f,
    };

    const VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        /* .pNext                 = */ nullptr,
        /* .flags                 = */ 0,
        /* .rasterizationSamples  = */ VK_SAMPLE_COUNT_1_BIT,  // desc.samples,
        /* .sampleShadingEnable   = */ VK_FALSE,
        /* .minSampleShading      = */ 1.0f,
        /* .pSampleMask           = */ nullptr,
        /* .alphaToCoverageEnable = */ VK_FALSE,
        /* .alphaToOneEnable      = */ VK_FALSE,
    };

    const VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        /* .pNext                 = */ nullptr,
        /* .flags                 = */ 0,
        /* .depthTestEnable       = */ desc.depth_test == DepthTest::Always ? VK_FALSE : VK_TRUE,
        /* .depthWriteEnable      = */ static_cast<VkBool32>(desc.depth_write),
        /* .depthCompareOp        = */ static_cast<VkCompareOp>(desc.depth_test),
        /* .depthBoundsTestEnable = */ false,
        /* .stencilTestEnable     = */ false,
        /* .front                 = */ {},
        /* .back                  = */ {},
        /* .minDepthBounds        = */ 0.0f,
        /* .maxDepthBounds        = */ 1.0f,
    };

    const VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
        /* .blendEnable         = */ VK_TRUE,
        /* .srcColorBlendFactor = */ static_cast<VkBlendFactor>(desc.blend_src),
        /* .dstColorBlendFactor = */ static_cast<VkBlendFactor>(desc.blend_dst),
        /* .colorBlendOp        = */ VK_BLEND_OP_ADD,
        /* .srcAlphaBlendFactor = */ VK_BLEND_FACTOR_ONE,
        /* .dstAlphaBlendFactor = */ VK_BLEND_FACTOR_ZERO,
        /* .alphaBlendOp        = */ VK_BLEND_OP_ADD,
        /* .colorWriteMask      = */ VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states(
        render_pass.attachment_count_, color_blend_attachment_state);
    const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        /* .pNext           = */ nullptr,
        /* .flags           = */ 0,
        /* .logicOpEnable   = */ VK_FALSE,
        /* .logicOp         = */ VK_LOGIC_OP_COPY,
        /* .attachmentCount = */ static_cast<uint32_t>(color_blend_attachment_states.size()),
        /* .pAttachments    = */ color_blend_attachment_states.data(),
        /* .blendConstants  = */ {0.0f, 0.0f, 0.0f, 0.0f},
    };

    const VkDynamicState dynamic_states[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    const VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        /* .pNext             = */ nullptr,
        /* .flags             = */ 0,
        /* .dynamicStateCount = */ 2,
        /* .pDynamicStates    = */ dynamic_states,
    };

    const VkGraphicsPipelineCreateInfo pipeline_create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        /* .pNext               = */ nullptr,
        /* .flags               = */ 0,
        /* .stageCount          = */ shader_stage_create_info_count,
        /* .pStages             = */ shader_stage_create_infos,
        /* .pVertexInputState   = */ &vertex_input_state_create_info,
        /* .pInputAssemblyState = */ &input_assembly_state_create_info,
        /* .pTessellationState  = */ nullptr,
        /* .pViewportState      = */ &viewport_state_create_info,
        /* .pRasterizationState = */ &rasterization_state_create_info,
        /* .pMultisampleState   = */ &multisample_state_create_info,
        /* .pDepthStencilState  = */ &depth_stencil_state_create_info,
        /* .pColorBlendState    = */ &color_blend_state_create_info,
        /* .pDynamicState       = */ &dynamic_state_create_info,
        /* .layout              = */ pipeline_layout_,
        /* .renderPass          = */ render_pass.render_pass_,
        /* .subpass             = */ 0,
        /* .basePipelineHandle  = */ VK_NULL_HANDLE,
        /* .basePipelineIndex   = */ -1,
    };

    VK_ASSERT(vkCreateGraphicsPipelines(device_, /*context.pipeline_cache_*/ nullptr, 1,
                                        &pipeline_create_info, nullptr, &pipeline_),
              "Failed to create pipeline");
  }
}

}  // namespace crystal::vulkan
