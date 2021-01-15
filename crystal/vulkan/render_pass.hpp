#pragma once

#include <array>

#include "absl/container/inlined_vector.h"
#include "crystal/common/render_pass_desc.hpp"
#include "crystal/vulkan/vk.hpp"

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                                        \
  VkDevice                              device_      = VK_NULL_HANDLE; \
  VkRenderPass                          render_pass_ = VK_NULL_HANDLE; \
  uint32_t                              attachment_count_ = 0;         \
  std::array<VkClearValue, 5>           clear_values_;                 \
  absl::InlinedVector<VkFramebuffer, 4> framebuffers_;                 \
  VkExtent2D                            extent_ = {};
#define CRYSTAL_IMPL_CTOR                               \
  RenderPass(Context& ctx);                             \
  RenderPass(Context& ctx, const RenderPassDesc& desc); \
                                                        \
  VkFramebuffer framebuffer(uint32_t swapchain_image_index) const noexcept;
#define CRYSTAL_IMPL_METHODS friend class ::crystal::vulkan::Pipeline;
#include "crystal/interface/render_pass.inl"

namespace crystal::vulkan {

inline VkFramebuffer RenderPass::framebuffer(uint32_t swapchain_image_index) const noexcept {
  if (framebuffers_.size() == 0) {
    return framebuffers_[0];
  }

  return framebuffers_[swapchain_image_index];
}

}  // namespace crystal::vulkan
