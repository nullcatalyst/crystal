#pragma once

#include <array>
#include <initializer_list>
#include <tuple>

#include "absl/container/inlined_vector.h"
#include "crystal/common/render_pass_desc.hpp"
#include "crystal/vulkan/vk.hpp"

namespace crystal::vulkan {

class Context;
class CommandBuffer;
class Pipeline;
class Texture;

class RenderPass {
  VkDevice                              device_           = VK_NULL_HANDLE;
  VkRenderPass                          render_pass_      = VK_NULL_HANDLE;
  uint32_t                              attachment_count_ = 0;
  std::array<VkClearValue, 5>           clear_values_;
  absl::InlinedVector<VkFramebuffer, 4> framebuffers_;
  VkExtent2D                            extent_ = {};

public:
  constexpr RenderPass() = default;

  RenderPass(const RenderPass&) = delete;
  RenderPass& operator=(const RenderPass&) = delete;

  RenderPass(RenderPass&& other);
  RenderPass& operator=(RenderPass&& other);

  ~RenderPass();

  [[nodiscard]] constexpr uint32_t width() const { return extent_.width; }
  [[nodiscard]] constexpr uint32_t height() const { return extent_.height; }

  void destroy();

private:
  friend class ::crystal::vulkan::Context;
  friend class ::crystal::vulkan::CommandBuffer;
  friend class ::crystal::vulkan::Pipeline;

  RenderPass(Context& ctx);
  RenderPass(
      Context&                                                                ctx,
      const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures);
  RenderPass(Context&                                                                ctx,
             const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
             const std::tuple<const Texture&, AttachmentDesc>                        depth_texture);

  VkFramebuffer framebuffer(uint32_t swapchain_image_index) const noexcept {
    if (framebuffers_.size() == 1) {
      return framebuffers_[0];
    }

    return framebuffers_[swapchain_image_index];
  }
};

}  // namespace crystal::vulkan
