#pragma once

#include <array>
#include <cstdint>

#include "crystal/vulkan/library.hpp"
#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                                           \
  VkDevice         device_          = VK_NULL_HANDLE;                     \
  VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;                     \
  VkRenderPass     render_pass_     = VK_NULL_HANDLE;                     \
                                                                          \
  VkDescriptorSetLayout          descriptor_set_layout_ = VK_NULL_HANDLE; \
  std::array<VkDescriptorSet, 4> descriptor_sets_;                        \
  VkPipelineLayout               pipeline_layout_ = VK_NULL_HANDLE;       \
  VkPipeline                     pipeline_        = VK_NULL_HANDLE;
#define CRYSTAL_IMPL_CTOR \
  Pipeline(Context& ctx, Library& library, RenderPass& render_pass, const PipelineDesc& desc);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/pipeline.inl"
