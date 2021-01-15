#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string_view>

#include "crystal/vulkan/command_buffer.hpp"
#include "crystal/vulkan/index_buffer.hpp"
#include "crystal/vulkan/internal/frame.hpp"
#include "crystal/vulkan/internal/swapchain.hpp"
#include "crystal/vulkan/library.hpp"
#include "crystal/vulkan/mesh.hpp"
#include "crystal/vulkan/pipeline.hpp"
#include "crystal/vulkan/render_pass.hpp"
#include "crystal/vulkan/texture.hpp"
#include "crystal/vulkan/uniform_buffer.hpp"
#include "crystal/vulkan/vertex_buffer.hpp"
#include "crystal/vulkan/vk.hpp"
#include "util/memory/ref_count.hpp"

#ifdef CRYSTAL_USE_SDL2
typedef struct SDL_Window SDL_Window;
#endif  // CRYSTAL_USE_SDL2

#define CRYSTAL_IMPL vulkan
#define CRYSTAL_IMPL_PROPERTIES                                      \
public:                                                              \
  struct Desc {                                                      \
    const char* application_name;                                    \
    uint32_t    max_descriptor_set_count;                            \
    uint32_t    buffer_descriptor_count;                             \
    uint32_t    texture_descriptor_count;                            \
  };                                                                 \
                                                                     \
private:                                                             \
  struct Buffer {                                                    \
    VkBuffer      buffer;                                            \
    VmaAllocation allocation;                                        \
                                                                     \
    constexpr Buffer(VkBuffer buffer, VmaAllocation allocation)      \
        : buffer(buffer), allocation(allocation) {}                  \
  };                                                                 \
                                                                     \
  VkInstance       instance_;                                        \
  VkSurfaceKHR     surface_;                                         \
  VkPhysicalDevice physical_device_;                                 \
  VkDevice         device_;                                          \
  VmaAllocator     memory_allocator_;                                \
  VkCommandPool    command_pool_;                                    \
  VkDescriptorPool descriptor_pool_;                                 \
                                                                     \
  ::util::unsafe::DeferredCtor<Texture>       screen_depth_texture_; \
  internal::Swapchain                         swapchain_;            \
  uint32_t                                    frame_index_ = 0;      \
  std::array<internal::Frame, 4>              frames_;               \
  std::vector<util::memory::RefCount<Buffer>> buffers_;
#ifdef CRYSTAL_USE_SDL2
#define CRYSTAL_IMPL_CTOR Context(const Desc& desc, SDL_Window* window);
#endif  // CRYSTAL_USE_SDL2
#define CRYSTAL_IMPL_METHODS                                            \
  friend class ::crystal::vulkan::internal::Swapchain;                  \
  friend class ::crystal::vulkan::internal::Frame;                      \
                                                                        \
  void add_buffer_(VkBuffer buffer, VmaAllocation allocation) noexcept; \
  void retain_buffer_(VkBuffer buffer) noexcept;                        \
  void release_buffer_(VkBuffer buffer) noexcept;
#include "crystal/interface/context.inl"

namespace crystal::vulkan {

inline void Context::wait() { vkDeviceWaitIdle(device_); }

inline Library Context::create_library(const std::string_view spv_path) {
  return Library(device_, std::string(spv_path));
}

inline RenderPass Context::create_render_pass(const RenderPassDesc& desc) {
  return RenderPass(*this, desc);
}

inline Pipeline Context::create_pipeline(Library& library, RenderPass& render_pass,
                                         const PipelineDesc& desc) {
  return Pipeline(*this, library, render_pass, desc);
}

inline UniformBuffer Context::create_uniform_buffer(size_t byte_length) {
  return UniformBuffer(*this, byte_length);
}

inline UniformBuffer Context::create_uniform_buffer(const void* const data_ptr,
                                                    const size_t      byte_length) {
  return UniformBuffer(*this, data_ptr, byte_length);
}

inline void Context::update_uniform_buffer(UniformBuffer&    uniform_buffer,
                                           const void* const data_ptr, const size_t byte_length) {
  uniform_buffer.update(data_ptr, byte_length);
}

inline VertexBuffer Context::create_vertex_buffer(size_t byte_length) {
  return VertexBuffer(*this, byte_length);
}

inline VertexBuffer Context::create_vertex_buffer(const void* const data_ptr,
                                                  const size_t      byte_length) {
  return VertexBuffer(*this, data_ptr, byte_length);
}

inline void Context::update_vertex_buffer(VertexBuffer& vertex_buffer, const void* const data_ptr,
                                          const size_t byte_length) {
  vertex_buffer.update(data_ptr, byte_length);
}

inline IndexBuffer Context::create_index_buffer(size_t byte_length) {
  return IndexBuffer(*this, byte_length);
}

inline IndexBuffer Context::create_index_buffer(const uint16_t* const data_ptr,
                                                const size_t          byte_length) {
  return IndexBuffer(*this, data_ptr, byte_length);
}

inline void Context::update_index_buffer(IndexBuffer& index_buffer, const uint16_t* const data_ptr,
                                         const size_t byte_length) {
  index_buffer.update(data_ptr, byte_length);
}

inline Mesh Context::create_mesh(
    const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings) {
  return Mesh(*this, bindings);
}

inline Mesh Context::create_mesh(
    const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
    const IndexBuffer&                                                     index_buffer) {
  return Mesh(*this, bindings, index_buffer);
}

}  // namespace crystal::vulkan
