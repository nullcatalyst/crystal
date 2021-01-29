#include "crystal/vulkan/uniform_buffer.hpp"

#include "crystal/vulkan/context.hpp"

namespace crystal::vulkan {

UniformBuffer::UniformBuffer(UniformBuffer&& other)
    : ctx_(other.ctx_),
      buffer_(other.buffer_),
      buffer_allocation_(other.buffer_allocation_),
      ptr_(other.ptr_),
      capacity_(other.capacity_) {
  other.ctx_               = nullptr;
  other.buffer_            = VK_NULL_HANDLE;
  other.buffer_allocation_ = VK_NULL_HANDLE;
  other.ptr_               = nullptr;
  other.capacity_          = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) {
  destroy();

  ctx_               = other.ctx_;
  buffer_            = other.buffer_;
  buffer_allocation_ = other.buffer_allocation_;
  ptr_               = other.ptr_;
  capacity_          = other.capacity_;

  other.ctx_               = nullptr;
  other.buffer_            = VK_NULL_HANDLE;
  other.buffer_allocation_ = VK_NULL_HANDLE;
  other.ptr_               = nullptr;
  other.capacity_          = 0;

  return *this;
}

UniformBuffer::~UniformBuffer() { destroy(); }

void UniformBuffer::destroy() noexcept {
  if (ctx_ == nullptr) {
    return;
  }

  vmaUnmapMemory(ctx_->memory_allocator_, buffer_allocation_);
  ctx_->release_buffer_(buffer_);

  ctx_               = nullptr;
  buffer_            = VK_NULL_HANDLE;
  buffer_allocation_ = VK_NULL_HANDLE;
  ptr_               = nullptr;
  capacity_          = 0;
}

void UniformBuffer::update(const void* const data_ptr, const size_t byte_length) noexcept {
  if (byte_length > capacity_) {
    util::msg::fatal("updating uniform buffer that has capacity [", capacity_,
                     "] with data that exceeds that capacity at length [", byte_length, "]");
  }

  memcpy(ptr_, data_ptr, byte_length);
  vmaFlushAllocation(ctx_->memory_allocator_, buffer_allocation_, 0, byte_length);
}

UniformBuffer::UniformBuffer(Context& ctx, const size_t byte_length)
    : ctx_(&ctx), capacity_(byte_length) {
  {  // Create buffer.
    const VkBufferCreateInfo buffer_info = {
        /* .sType = */ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /* .pNext                 = */ nullptr,
        /* .flags                 = */ 0,
        /* .size                  = */ byte_length,
        /* .usage                 = */ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        /* .sharingMode           = */ VK_SHARING_MODE_EXCLUSIVE,
        /* .queueFamilyIndexCount = */ 0,
        /* .pQueueFamilyIndices   = */ nullptr,
    };

    const VmaAllocationCreateInfo alloc_info = {
        /* .flags          = */ 0,
        /* .usage          = */ VMA_MEMORY_USAGE_CPU_TO_GPU,
        /* .requiredFlags  = */ 0,
        /* .preferredFlags = */ 0,
        /* .memoryTypeBits = */ 0,
        /* .pool           = */ VK_NULL_HANDLE,
        /* .pUserData      = */ nullptr,
    };

    VK_ASSERT(vmaCreateBuffer(ctx_->memory_allocator_, &buffer_info, &alloc_info, &buffer_,
                              &buffer_allocation_, nullptr),
              "allocating uniform buffer");

    ctx_->add_buffer_(buffer_, buffer_allocation_);
  }

  {  // Get pointer to staging buffer memory.
    VK_ASSERT(vmaMapMemory(ctx_->memory_allocator_, buffer_allocation_, &ptr_),
              "getting pointer to staging buffer");
  }
}

UniformBuffer::UniformBuffer(Context& ctx, const void* const data_ptr, const size_t byte_length)
    : UniformBuffer(ctx, byte_length) {
  update(data_ptr, byte_length);
}

}  // namespace crystal::vulkan
