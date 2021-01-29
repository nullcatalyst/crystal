#include "crystal/vulkan/command_buffer.hpp"

#include "crystal/vulkan/context.hpp"
#include "crystal/vulkan/render_pass.hpp"

namespace crystal::vulkan {

CommandBuffer::CommandBuffer(Context& ctx, internal::Frame& frame, uint32_t frame_index)
    : command_buffer_(frame.command_buffer_),
      swapchain_image_(ctx.swapchain_.images_[frame.swapchain_image_index_].image),
      swapchain_image_view_(ctx.swapchain_.images_[frame.swapchain_image_index_].view),
      swapchain_image_format_(ctx.swapchain_.create_info_.imageFormat),
      swapchain_image_extent_(ctx.swapchain_.create_info_.imageExtent),
      device_(ctx.device_),
      swapchain_(ctx.swapchain_.swapchain_),
      graphics_queue_(ctx.swapchain_.graphics_queue_),
      present_queue_(ctx.swapchain_.present_queue_),
      image_available_semaphore_(frame.image_available_semaphore_),
      rendering_complete_semaphore_(frame.rendering_complete_semaphore_),
      fence_(frame.fence_),
      swapchain_image_index_(frame.swapchain_image_index_),
      frame_index_(frame_index) {}

CommandBuffer::~CommandBuffer() {
  vkCmdEndRenderPass(command_buffer_);
  VK_ASSERT(vkEndCommandBuffer(command_buffer_), "ending command buffer");

  const VkPipelineStageFlags wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  const VkSubmitInfo         submit_info    = {
      /* sType */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
      /* pNext                */ nullptr,
      /* waitSemaphoreCount   */ 1,
      /* pWaitSemaphores      */ &image_available_semaphore_,
      /* pWaitDstStageMask    */ wait_stages,
      /* commandBufferCount   */ 1,
      /* pCommandBuffers      */ &command_buffer_,
      /* signalSemaphoreCount */ 1,
      /* pSignalSemaphores    */ &rendering_complete_semaphore_,
  };

  VK_ASSERT(vkQueueSubmit(graphics_queue_, 1, &submit_info, fence_), "submitting render queue");

#ifdef __ggp__
  const VkPresentFrameTokenGGP frame_token_metadata = {
      /* sType */ VK_STRUCTURE_TYPE_PRESENT_FRAME_TOKEN_GGP,
      /* pNext      */ nullptr,
      /* frameToken */ frame_token_,
  };

  const VkPresentInfoKHR present_info = {
      /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      /* pNext              */ &frame_token_metadata,
      /* waitSemaphoreCount */ 1,
      /* pWaitSemaphores    */ &rendering_complete_semaphore_,
      /* swapchainCount     */ 1,
      /* pSwapchains        */ &swapchain_,
      /* pImageIndices      */ &image_index_,
      /* pResults           */ nullptr,
  };
#else
  const VkPresentInfoKHR present_info = {
      /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      /* pNext              */ nullptr,
      /* waitSemaphoreCount */ 1,
      /* pWaitSemaphores    */ &rendering_complete_semaphore_,
      /* swapchainCount     */ 1,
      /* pSwapchains        */ &swapchain_,
      /* pImageIndices      */ &swapchain_image_index_,
      /* pResults           */ nullptr,
  };
#endif  // __ggp__

  VK_ASSERT(vkQueuePresentKHR(present_queue_, &present_info), "presenting queue");
}

void CommandBuffer::use_render_pass(RenderPass& render_pass) {
  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  VkExtent2D    extent      = render_pass.extent_;

  {  // Begin render pass.
    VkFramebuffer framebuffer = render_pass.framebuffer(swapchain_image_index_);

    const VkRenderPassBeginInfo begin_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        /* .pNext           = */ nullptr,
        /* .renderPass      = */ render_pass.render_pass_,
        /* .framebuffer     = */ framebuffer,
        /* .renderArea      = */
        {
            {0, 0},
            extent,
        },
        /* .clearValueCount = */ render_pass.attachment_count_,
        /* .pClearValues    = */ render_pass.clear_values_.data(),
    };

    vkCmdBeginRenderPass(command_buffer_, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    // Update dynamic viewport state.
    const VkViewport viewport = {
        /* .x        = */ 0.0f,
        /* .y        = */ 0.0f,
        /* .width    = */ static_cast<float>(extent.width),
        /* .height   = */ static_cast<float>(extent.height),
        /* .minDepth = */ 0.0f,
        /* .maxDepth = */ 1.0f,
    };
    vkCmdSetViewport(command_buffer_, 0, 1, &viewport);

    // Update dynamic scissor state.
    VkRect2D scissor = {
        /* .x       = */ 0,
        /* .y       = */ 0,
        /* .width   = */ render_pass.extent_.width,
        /* .height  = */ render_pass.extent_.height,
    };
    vkCmdSetScissor(command_buffer_, 0, 1, &scissor);
  }
}

void CommandBuffer::use_pipeline(Pipeline& pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline_);

  pipeline_layout_ = pipeline.pipeline_layout_;
  descriptor_set_  = pipeline.descriptor_sets_[frame_index_];
}

void CommandBuffer::use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t location,
                                       uint32_t binding) {
  {  // Update the descriptor set.
    const VkDescriptorBufferInfo buffer_info = {
        /* .buffer = */ uniform_buffer.buffer_,
        /* .offset = */ 0,
        /* .range  = */ VK_WHOLE_SIZE,
    };
    const VkWriteDescriptorSet write_descriptor = {
        /* .sType = */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        /* .pNext            = */ nullptr,
        /* .dstSet           = */ descriptor_set_,
        /* .dstBinding       = */ location,
        /* .dstArrayElement  = */ 0,
        /* .descriptorCount  = */ 1,
        /* .descriptorType   = */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        /* .pImageInfo       = */ nullptr,
        /* .pBufferInfo      = */ &buffer_info,
        /* .pTexelBufferView = */ nullptr,
    };

    vkUpdateDescriptorSets(device_, 1, &write_descriptor, 0, nullptr);
  }

  update_descriptor_set_ = true;
}

void CommandBuffer::draw(Mesh& mesh, uint32_t vertex_count, uint32_t instance_count) {
  if (update_descriptor_set_) {
    vkCmdBindDescriptorSets(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0,
                            1, &descriptor_set_, 0, nullptr);
  }

  const VkDeviceSize offsets[] = {0};
  for (uint32_t i = 0; i < mesh.binding_count_; ++i) {
    vkCmdBindVertexBuffers(command_buffer_, mesh.bindings_[i].index, 1, &mesh.bindings_[i].buffer,
                           offsets);
  }

  if (mesh.index_buffer_ != VK_NULL_HANDLE) {
    vkCmdBindIndexBuffer(command_buffer_, mesh.index_buffer_, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer_, vertex_count, instance_count, 0, 0, 0);
  } else {
    vkCmdDraw(command_buffer_, vertex_count, instance_count, 0, 0);
  }
}

}  // namespace crystal::vulkan
