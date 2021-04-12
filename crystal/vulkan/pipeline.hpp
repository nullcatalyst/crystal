#pragma once

#include <array>

#include "crystal/common/pipeline_desc.hpp"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;
class Library;
class RenderPass;

class Pipeline {
  VkDevice         device_          = VK_NULL_HANDLE;
  VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
  VkRenderPass     render_pass_     = VK_NULL_HANDLE;

  VkDescriptorSetLayout          uniform_descriptor_set_layout_ = VK_NULL_HANDLE;
  std::array<VkDescriptorSet, 4> uniform_descriptor_sets_;
  VkDescriptorSetLayout          texture_descriptor_set_layout_ = VK_NULL_HANDLE;
  std::array<VkDescriptorSet, 4> texture_descriptor_sets_;
  VkPipelineLayout               pipeline_layout_ = VK_NULL_HANDLE;
  VkPipeline                     pipeline_        = VK_NULL_HANDLE;

public:
  Pipeline() = default;

  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  Pipeline(Pipeline&& other);
  Pipeline& operator=(Pipeline&& other);

  ~Pipeline();

  void destroy() noexcept;

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;

  Pipeline(Context& ctx, Library& library, RenderPass& render_pass, const PipelineDesc& desc);
};

}  // namespace crystal::vulkan
