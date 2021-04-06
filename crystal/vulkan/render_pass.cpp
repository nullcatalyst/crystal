#include "crystal/vulkan/render_pass.hpp"

#include <limits>

#include "crystal/vulkan/command_buffer.hpp"
#include "crystal/vulkan/context.hpp"
#include "crystal/vulkan/texture.hpp"

namespace crystal::vulkan {

RenderPass::RenderPass(RenderPass&& other)
    : device_(other.device_),
      render_pass_(other.render_pass_),
      attachment_count_(other.attachment_count_),
      has_depth_(other.has_depth_),
      clear_values_(std::move(other.clear_values_)),
      framebuffers_(std::move(other.framebuffers_)),
      extent_(other.extent_) {
  other.device_           = VK_NULL_HANDLE;
  other.render_pass_      = VK_NULL_HANDLE;
  other.attachment_count_ = 0;
  other.has_depth_        = false;
  other.clear_values_     = {};
  other.framebuffers_.resize(0);
  other.extent_ = {};
}

RenderPass& RenderPass::operator=(RenderPass&& other) {
  destroy();

  device_           = other.device_;
  render_pass_      = other.render_pass_;
  attachment_count_ = other.attachment_count_;
  has_depth_        = other.has_depth_;
  clear_values_     = std::move(other.clear_values_);
  framebuffers_     = std::move(other.framebuffers_);
  extent_           = other.extent_;

  other.device_           = VK_NULL_HANDLE;
  other.render_pass_      = VK_NULL_HANDLE;
  other.attachment_count_ = 0;
  other.has_depth_        = false;
  other.clear_values_     = {};
  other.framebuffers_.resize(0);
  other.extent_ = {};

  return *this;
}

RenderPass::~RenderPass() { destroy(); }

void RenderPass::destroy() {
  if (device_ == nullptr) {
    return;
  }

  for (auto framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device_, framebuffer, nullptr);
  }
  vkDestroyRenderPass(device_, render_pass_, nullptr);

  device_           = VK_NULL_HANDLE;
  render_pass_      = VK_NULL_HANDLE;
  attachment_count_ = 0;
  has_depth_        = false;
  framebuffers_.resize(0);
  extent_ = {};
}

RenderPass::RenderPass(Context& ctx) : device_(ctx.device_) {
  attachment_count_ = 1;
  has_depth_        = true;
  clear_values_     = {
      VkClearValue{
          .color = {0.0f, 0.0f, 0.0f, 0.0f},
      },
      VkClearValue{
          .depthStencil = {
              /* .depth   = */ 1.0f,
              /* .stencil = */ 0,
          },
      },
  };

  {  // Create render pass.
    std::array<VkAttachmentDescription, 2> attachment_descriptions{
        VkAttachmentDescription{
            /* .flags          = */ 0,
            /* .format         = */ ctx.swapchain_.create_info_.imageFormat,
            /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
            /* .loadOp         = */ VK_ATTACHMENT_LOAD_OP_CLEAR,
            /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
            /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
            /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
            /* .finalLayout    = */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
        VkAttachmentDescription{
            /* .flags          = */ 0,
            /* .format         = */ ctx.screen_depth_texture_.format_,
            /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
            /* .loadOp         = */ VK_ATTACHMENT_LOAD_OP_CLEAR,
            /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
            /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
            /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
            /* .finalLayout    = */ ctx.screen_depth_texture_.layout_,
        },
    };
    std::array<VkAttachmentReference, 2> attachment_references{
        VkAttachmentReference{
            /* .attachment = */ 0,
            /* .layout     = */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
        VkAttachmentReference{
            /* .attachment = */ 1,
            /* .layout     = */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
    };

    const VkSubpassDescription subpasses[] = {
        {
            /* .flags                   = */ 0,
            /* .pipelineBindPoint       = */ VK_PIPELINE_BIND_POINT_GRAPHICS,
            /* .inputAttachmentCount    = */ 0,
            /* .pInputAttachments       = */ nullptr,
            /* .colorAttachmentCount    = */ 1,
            /* .pColorAttachments       = */ attachment_references.data(),
            /* .pResolveAttachments     = */ nullptr,
            /* .pDepthStencilAttachment = */ &attachment_references[1],
            /* .preserveAttachmentCount = */ 0,
            /* .pPreserveAttachments    = */ nullptr,
        },
    };

    const VkRenderPassCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        /* .pNext           = */ nullptr,
        /* .flags           = */ 0,
        /* .attachmentCount = */ 2,
        /* .pAttachments    = */ attachment_descriptions.data(),
        /* .subpassCount    = */ 1,
        /* .pSubpasses      = */ subpasses,
        /* .dependencyCount = */ 0,
        /* .pDependencies   = */ nullptr,
    };

    VK_ASSERT(vkCreateRenderPass(device_, &create_info, nullptr, &render_pass_),
              "creating render pass");
  }

  {  // Create framebuffer.
    std::array<VkImageView, 2> attachment_views;
    extent_ = ctx.swapchain_.create_info_.imageExtent;

    attachment_views[1] = ctx.screen_depth_texture_.image_view_;

    framebuffers_.resize(ctx.swapchain_.images_.size());
    for (uint32_t i = 0; i < ctx.swapchain_.images_.size(); ++i) {
      attachment_views[0] = ctx.swapchain_.images_[i].view;

      const VkFramebufferCreateInfo create_info = {
          /* .sType = */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
          /* .pNext           = */ nullptr,
          /* .flags           = */ 0,
          /* .renderPass      = */ render_pass_,
          /* .attachmentCount = */ 2,
          /* .pAttachments    = */ attachment_views.data(),
          /* .width           = */ extent_.width,
          /* .height          = */ extent_.height,
          /* .layers          = */ 1,
      };

      VK_ASSERT(vkCreateFramebuffer(device_, &create_info, nullptr, &framebuffers_[i]),
                "creating framebuffer");
    }
  }
}

RenderPass::RenderPass(
    Context&                                                                ctx,
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures)
    : device_(ctx.device_), attachment_count_(0), has_depth_(false) {
  std::array<VkImageView, 5> attachment_views;

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = color_textures.begin()[0];
    extent_                     = texture.extent_;
  }

  {  // Create render pass.
    std::array<VkAttachmentDescription, 5> attachment_descriptions;
    std::array<VkAttachmentReference, 5>   attachment_references;

    for (uint32_t i = 0; i < color_textures.size(); ++i) {
      const auto& [texture, desc] = color_textures.begin()[i];

      if (extent_.width != texture.extent_.width || extent_.height != texture.extent_.height) {
        util::msg::fatal(
            "all textures in a framebuffer must be of the same size: framebuffer width=",
            extent_.width, " height=", extent_.height, ", texture width=", texture.extent_.width,
            " height=", texture.extent_.height);
      }

      attachment_descriptions[attachment_count_] = VkAttachmentDescription{
          /* .flags          = */ 0,
          /* .format         = */ texture.format_,
          /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
          /* .loadOp         = */ desc.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                             : VK_ATTACHMENT_LOAD_OP_LOAD,
          /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
          /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
          /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
          /* .finalLayout    = */ VK_IMAGE_LAYOUT_GENERAL,
      };

      attachment_references[attachment_count_] = VkAttachmentReference{
          /* .attachment = */ attachment_count_,
          /* .layout     = */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      };

      clear_values_[attachment_count_] = VkClearValue{
          .color =
              {
                  desc.clear_value.color.red,
                  desc.clear_value.color.green,
                  desc.clear_value.color.blue,
                  desc.clear_value.color.alpha,
              },
      };

      attachment_views[attachment_count_] = texture.image_view_;

      ++attachment_count_;
    }

    const VkSubpassDescription subpasses[] = {
        {
            /* .flags                   = */ 0,
            /* .pipelineBindPoint       = */ VK_PIPELINE_BIND_POINT_GRAPHICS,
            /* .inputAttachmentCount    = */ 0,
            /* .pInputAttachments       = */ nullptr,
            /* .colorAttachmentCount    = */ attachment_count_,
            /* .pColorAttachments       = */ attachment_references.data(),
            /* .pResolveAttachments     = */ nullptr,
            /* .pDepthStencilAttachment = */ nullptr,
            /* .preserveAttachmentCount = */ 0,
            /* .pPreserveAttachments    = */ nullptr,
        },
    };

    const std::array<VkSubpassDependency, 2> subpass_dependencies{
        VkSubpassDependency{
            /* srcSubpass      = */ VK_SUBPASS_EXTERNAL,
            /* dstSubpass      = */ 0,
            /* srcStageMask    = */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            /* dstStageMask    = */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /* srcAccessMask   = */ VK_ACCESS_SHADER_READ_BIT,
            /* dstAccessMask   = */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /* dependencyFlags = */ VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency{
            /* srcSubpass      = */ 0,
            /* dstSubpass      = */ VK_SUBPASS_EXTERNAL,
            /* srcStageMask    = */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /* dstStageMask    = */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            /* srcAccessMask   = */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /* dstAccessMask   = */ VK_ACCESS_SHADER_READ_BIT,
            /* dependencyFlags = */ VK_DEPENDENCY_BY_REGION_BIT,
        },
    };

    const VkRenderPassCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        /* .pNext           = */ nullptr,
        /* .flags           = */ 0,
        /* .attachmentCount = */ attachment_count_,
        /* .pAttachments    = */ attachment_descriptions.data(),
        /* .subpassCount    = */ 1,
        /* .pSubpasses      = */ subpasses,
        /* .dependencyCount = */ static_cast<uint32_t>(subpass_dependencies.size()),
        /* .pDependencies   = */ subpass_dependencies.data(),
    };

    VK_ASSERT(vkCreateRenderPass(device_, &create_info, nullptr, &render_pass_),
              "creating render pass");
  }

  {  // Create framebuffer.
    framebuffers_.resize(1);
    const VkFramebufferCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        /* .pNext           = */ nullptr,
        /* .flags           = */ 0,
        /* .renderPass      = */ render_pass_,
        /* .attachmentCount = */ attachment_count_,
        /* .pAttachments    = */ attachment_views.data(),
        /* .width           = */ extent_.width,
        /* .height          = */ extent_.height,
        /* .layers          = */ 1,
    };

    VK_ASSERT(vkCreateFramebuffer(device_, &create_info, nullptr, &framebuffers_[0]),
              "creating framebuffer");
  }
}

RenderPass::RenderPass(
    Context&                                                                ctx,
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
    const std::tuple<const Texture&, AttachmentDesc>                        depth_texture)
    : device_(ctx.device_), attachment_count_(0), has_depth_(true) {
  std::array<VkImageView, 5> attachment_views;

  {  // Save the dimensions of the framebuffer.
    const auto& [texture, desc] = depth_texture;
    extent_                     = texture.extent_;
  }

  {  // Create render pass.
    std::array<VkAttachmentDescription, 5> attachment_descriptions;
    std::array<VkAttachmentReference, 5>   attachment_references;

    for (uint32_t i = 0; i < color_textures.size(); ++i) {
      const auto& [texture, desc] = color_textures.begin()[i];

      if (extent_.width != texture.extent_.width || extent_.height != texture.extent_.height) {
        util::msg::fatal(
            "all textures in a framebuffer must be of the same size: framebuffer width=",
            extent_.width, " height=", extent_.height, ", texture width=", texture.extent_.width,
            " height=", texture.extent_.height);
      }

      attachment_descriptions[attachment_count_] = VkAttachmentDescription{
          /* .flags          = */ 0,
          /* .format         = */ texture.format_,
          /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
          /* .loadOp         = */ desc.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                             : VK_ATTACHMENT_LOAD_OP_LOAD,
          /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
          /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
          /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
          /* .finalLayout    = */ VK_IMAGE_LAYOUT_GENERAL,
      };

      attachment_references[attachment_count_] = VkAttachmentReference{
          /* .attachment = */ attachment_count_,
          /* .layout     = */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      };

      clear_values_[attachment_count_] = VkClearValue{
          .color =
              {
                  desc.clear_value.color.red,
                  desc.clear_value.color.green,
                  desc.clear_value.color.blue,
                  desc.clear_value.color.alpha,
              },
      };

      attachment_views[attachment_count_] = texture.image_view_;

      ++attachment_count_;
    }

    {  // Add depth attachment.
      const auto& [texture, desc] = depth_texture;

      attachment_descriptions[attachment_count_] = VkAttachmentDescription{
          /* .flags          = */ 0,
          /* .format         = */ texture.format_,
          /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
          /* .loadOp         = */ desc.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                             : VK_ATTACHMENT_LOAD_OP_LOAD,
          /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
          /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
          /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
          /* .finalLayout    = */ VK_IMAGE_LAYOUT_GENERAL,
      };

      attachment_references[attachment_count_] = VkAttachmentReference{
          /* .attachment = */ attachment_count_,
          /* .layout     = */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      };

      clear_values_[attachment_count_] = VkClearValue{
          .depthStencil = {
              /* .depth   = */ desc.clear_value.depth,
              /* .stencil = */ 0,
          },
      };

      attachment_views[attachment_count_] = texture.image_view_;
    }

    const VkSubpassDescription subpasses[] = {
        {
            /* .flags                   = */ 0,
            /* .pipelineBindPoint       = */ VK_PIPELINE_BIND_POINT_GRAPHICS,
            /* .inputAttachmentCount    = */ 0,
            /* .pInputAttachments       = */ nullptr,
            /* .colorAttachmentCount    = */ attachment_count_,
            /* .pColorAttachments       = */ attachment_references.data(),
            /* .pResolveAttachments     = */ nullptr,
            /* .pDepthStencilAttachment = */ &attachment_references[attachment_count_],
            /* .preserveAttachmentCount = */ 0,
            /* .pPreserveAttachments    = */ nullptr,
        },
    };

    const VkSubpassDependency subpass_dependencies[] = {
        VkSubpassDependency{
            /* srcSubpass      = */ VK_SUBPASS_EXTERNAL,
            /* dstSubpass      = */ 0,
            /* srcStageMask    = */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            /* dstStageMask    = */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /* srcAccessMask   = */ VK_ACCESS_SHADER_READ_BIT,
            /* dstAccessMask   = */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /* dependencyFlags = */ VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency{
            /* srcSubpass      = */ 0,
            /* dstSubpass      = */ VK_SUBPASS_EXTERNAL,
            /* srcStageMask    = */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /* dstStageMask    = */ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            /* srcAccessMask   = */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /* dstAccessMask   = */ VK_ACCESS_SHADER_READ_BIT,
            /* dependencyFlags = */ VK_DEPENDENCY_BY_REGION_BIT,
        },
    };

    const VkRenderPassCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        /* .pNext           = */ nullptr,
        /* .flags           = */ 0,
        /* .attachmentCount = */ attachment_count_ + 1,
        /* .pAttachments    = */ attachment_descriptions.data(),
        /* .subpassCount    = */ 1,
        /* .pSubpasses      = */ subpasses,
        /* .dependencyCount = */ 2,
        /* .pDependencies   = */ subpass_dependencies,
    };

    VK_ASSERT(vkCreateRenderPass(device_, &create_info, nullptr, &render_pass_),
              "creating render pass");
  }

  {  // Create framebuffer.
    framebuffers_.resize(1);
    const VkFramebufferCreateInfo create_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        /* .pNext           = */ nullptr,
        /* .flags           = */ 0,
        /* .renderPass      = */ render_pass_,
        /* .attachmentCount = */ attachment_count_ + 1,
        /* .pAttachments    = */ attachment_views.data(),
        /* .width           = */ extent_.width,
        /* .height          = */ extent_.height,
        /* .layers          = */ 1,
    };

    VK_ASSERT(vkCreateFramebuffer(device_, &create_info, nullptr, &framebuffers_[0]),
              "creating framebuffer");
  }
}

}  // namespace crystal::vulkan
