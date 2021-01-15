#include "crystal/vulkan/render_pass.hpp"

#include <limits>

#include "crystal/vulkan/command_buffer.hpp"
#include "crystal/vulkan/context.hpp"

namespace crystal::vulkan {

RenderPass::RenderPass(RenderPass&& other)
    : device_(other.device_),
      render_pass_(other.render_pass_),
      framebuffers_(std::move(other.framebuffers_)),
      extent_(other.extent_) {
  other.device_      = VK_NULL_HANDLE;
  other.render_pass_ = VK_NULL_HANDLE;
  other.extent_      = {};
}

RenderPass& RenderPass::operator=(RenderPass&& other) {
  for (auto framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device_, framebuffer, nullptr);
  }
  vkDestroyRenderPass(device_, render_pass_, nullptr);

  device_       = other.device_;
  render_pass_  = other.render_pass_;
  framebuffers_ = std::move(other.framebuffers_);
  extent_       = other.extent_;

  other.device_      = VK_NULL_HANDLE;
  other.render_pass_ = VK_NULL_HANDLE;
  other.extent_      = {};

  return *this;
}

RenderPass::~RenderPass() {
  if (device_ == VK_NULL_HANDLE) {
    return;
  }

  for (auto framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device_, framebuffer, nullptr);
  }
  vkDestroyRenderPass(device_, render_pass_, nullptr);
}

RenderPass::RenderPass(Context& ctx) : device_(ctx.device_) {
  attachment_count_ = 2;
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
            /* .format         = */ ctx.screen_depth_texture_->format_,
            /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
            /* .loadOp         = */ VK_ATTACHMENT_LOAD_OP_CLEAR,
            /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
            /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
            /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
            /* .finalLayout    = */ ctx.screen_depth_texture_->layout_,
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
        /* .attachmentCount = */ attachment_count_,
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

    attachment_views[1] = ctx.screen_depth_texture_->image_view_;

    framebuffers_.resize(ctx.swapchain_.images_.size());
    for (uint32_t i = 0; i < ctx.swapchain_.images_.size(); ++i) {
      attachment_views[0] = ctx.swapchain_.images_[i].view;

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

      VK_ASSERT(vkCreateFramebuffer(device_, &create_info, nullptr, &framebuffers_[i]),
                "creating framebuffer");
    }
  }
}

// TODO: Remove screen code.
// RenderPass::RenderPass(Context& ctx, const RenderPassDesc& desc) : device_(ctx.device_) {
//   uint32_t                    attachment_count = 0;
//   std::array<VkClearValue, 5> clear_values;
//   std::array<VkImageView, 5>  attachment_views;

//   {  // Create render pass.
//     std::array<VkAttachmentDescription, 5> attachment_descriptions;
//     std::array<VkAttachmentReference, 5>   attachment_references;

//     for (uint32_t i = 0; i < desc.attachments.size(); ++i) {
//       const auto& attachment = desc.attachments.begin()[i];

//       attachment_descriptions[attachment_count] = VkAttachmentDescription{
//           /* .flags          = */ 0,
//           /* .format         = */ attachment.screen ? ctx.swapchain_.create_info_.imageFormat
//                                                     : VK_FORMAT_UNDEFINED,
//           /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
//           /* .loadOp         = */ attachment.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
//                                                    : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//           /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
//           /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//           /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
//           /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
//           /* .finalLayout    = */ attachment.screen ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
//                                                     : VK_IMAGE_LAYOUT_UNDEFINED,
//       };

//       attachment_references[attachment_count] = VkAttachmentReference{
//           /* .attachment = */ attachment_count,
//           /* .layout     = */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//       };

//       clear_values[attachment_count] = VkClearValue{
//           .color =
//               {
//                   attachment.clear_value.red,
//                   attachment.clear_value.green,
//                   attachment.clear_value.blue,
//                   attachment.clear_value.alpha,
//               },
//       };

//       attachment_views[attachment_count] = VK_NULL_HANDLE;  //< Set as screen.

//       ++attachment_count;
//     }

//     if (desc.depth_attachment.use_depth) {
//       attachment_descriptions[attachment_count] = VkAttachmentDescription{
//           /* .flags          = */ 0,
//           /* .format         = */ VK_FORMAT_D32_SFLOAT,
//           /* .samples        = */ VK_SAMPLE_COUNT_1_BIT,
//           /* .loadOp         = */ VK_ATTACHMENT_LOAD_OP_CLEAR,
//           /* .storeOp        = */ VK_ATTACHMENT_STORE_OP_STORE,
//           /* .stencilLoadOp  = */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
//           /* .stencilStoreOp = */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
//           /* .initialLayout  = */ VK_IMAGE_LAYOUT_UNDEFINED,
//           /* .finalLayout    = */ VK_IMAGE_LAYOUT_GENERAL,
//       };

//       attachment_references[attachment_count] = VkAttachmentReference{
//           /* .attachment = */ attachment_count,
//           /* .layout     = */ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//       };

//       clear_values[attachment_count] = VkClearValue{
//           .depthStencil = {
//               /* .depth   = */ desc.depth_attachment.clear_depth,
//               /* .stencil = */ 0,
//           },
//       };

//       // TODO: flag screen depth differently somehow.
//       attachment_views[attachment_count] = nullptr;

//       ++attachment_count;
//     }

//     const VkSubpassDescription subpasses[] = {
//         {
//             /* .flags                   = */ 0,
//             /* .pipelineBindPoint       = */ VK_PIPELINE_BIND_POINT_GRAPHICS,
//             /* .inputAttachmentCount    = */ 0,
//             /* .pInputAttachments       = */ nullptr,
//             /* .colorAttachmentCount    = */ attachment_count,
//             /* .pColorAttachments       = */ attachment_references.data(),
//             /* .pResolveAttachments     = */ nullptr,
//             /* .pDepthStencilAttachment = */ desc.depth_attachment.use_depth
//                 ? &attachment_references[attachment_count - 1]
//                 : nullptr,
//             /* .preserveAttachmentCount = */ 0,
//             /* .pPreserveAttachments    = */ nullptr,
//         },
//     };

//     const VkRenderPassCreateInfo create_info = {
//         /* .sType = */ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//         /* .pNext           = */ nullptr,
//         /* .flags           = */ 0,
//         /* .attachmentCount = */ attachment_count,
//         /* .pAttachments    = */ attachment_descriptions.data(),
//         /* .subpassCount    = */ 1,
//         /* .pSubpasses      = */ subpasses,
//         /* .dependencyCount = */ 0,
//         /* .pDependencies   = */ nullptr,
//     };

//     VK_ASSERT(vkCreateRenderPass(device_, &create_info, nullptr, &render_pass_),
//               "creating render pass");
//   }

//   {  // Create framebuffer.
//     uint32_t                   screen_index = ~0;
//     std::array<VkImageView, 5> attachment_views;
//     for (uint32_t i = 0; i < attachment_count; ++i) {
//       // attachment_views[i] = render_pass.attachment_views_[i] != nullptr
//       //                           ? render_pass.attachment_views_[i]
//       //                           : frame_.image_view_;

//       // TODO: Handle non-screen variant.
//       attachment_views[i] = nullptr;
//       screen_index        = i;
//     }

//     if (screen_index != ~0) {
//       // The screen is being rendered to in this render pass, we need to create multiple
//       // framebuffers for each image in the swapchain.

//       contains_screen_ = true;
//       extent_          = ctx.swapchain_.create_info_.imageExtent;

//       framebuffers_.resize(ctx.swapchain_.images_.size());
//       for (uint32_t i = 0; i < ctx.swapchain_.images_.size(); ++i) {
//         attachment_views[screen_index] = ctx.swapchain_.images_[i].view;

//         const VkFramebufferCreateInfo create_info = {
//             /* .sType = */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//             /* .pNext           = */ nullptr,
//             /* .flags           = */ 0,
//             /* .renderPass      = */ render_pass_,
//             /* .attachmentCount = */ attachment_count,
//             /* .pAttachments    = */ attachment_views.data(),
//             /* .width           = */ extent_.width,
//             /* .height          = */ extent_.height,
//             /* .layers          = */ 1,
//         };

//         VK_ASSERT(vkCreateFramebuffer(device_, &create_info, nullptr, &framebuffers_[i]),
//                   "creating framebuffer");
//       }
//     } else {
//       extent_ = {};

//       framebuffers_.resize(1);
//       const VkFramebufferCreateInfo create_info = {
//           /* .sType = */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//           /* .pNext           = */ nullptr,
//           /* .flags           = */ 0,
//           /* .renderPass      = */ render_pass_,
//           /* .attachmentCount = */ attachment_count,
//           /* .pAttachments    = */ attachment_views.data(),
//           /* .width           = */ extent_.width,
//           /* .height          = */ extent_.height,
//           /* .layers          = */ 1,
//       };

//       VK_ASSERT(vkCreateFramebuffer(device_, &create_info, nullptr, &framebuffers_[0]),
//                 "creating framebuffer");
//     }
//   }
// }

}  // namespace crystal::vulkan
