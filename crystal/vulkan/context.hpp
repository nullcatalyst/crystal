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

#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

namespace crystal::vulkan {

class CommandBuffer;
class IndexBuffer;
class Library;
class Mesh;
class Pipeline;
class RenderPass;
class Texture;
class UniformBuffer;
class VertexBuffer;

class Context {
public:
  using CommandBuffer = ::crystal::vulkan::CommandBuffer;
  using IndexBuffer   = ::crystal::vulkan::IndexBuffer;
  using Library       = ::crystal::vulkan::Library;
  using Mesh          = ::crystal::vulkan::Mesh;
  using Pipeline      = ::crystal::vulkan::Pipeline;
  using RenderPass    = ::crystal::vulkan::RenderPass;
  using Texture       = ::crystal::vulkan::Texture;
  using UniformBuffer = ::crystal::vulkan::UniformBuffer;
  using VertexBuffer  = ::crystal::vulkan::VertexBuffer;

#ifdef CRYSTAL_USE_SDL2

  struct Desc {
    SDL_Window* window;
    const char* application_name;
    uint32_t    max_descriptor_set_count;
    uint32_t    buffer_descriptor_count;
    uint32_t    texture_descriptor_count;
  };

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

  struct Desc {
    const char* application_name;
    uint32_t    max_descriptor_set_count;
    uint32_t    buffer_descriptor_count;
    uint32_t    texture_descriptor_count;
  };

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)

private:
  struct Buffer {
    uint32_t      ref_count;
    VkBuffer      buffer;
    VmaAllocation allocation;

    constexpr Buffer(VkBuffer buffer, VmaAllocation allocation)
        : ref_count(1), buffer(buffer), allocation(allocation) {}
  };

  VkInstance       instance_         = VK_NULL_HANDLE;
  VkSurfaceKHR     surface_          = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_  = VK_NULL_HANDLE;
  VkDevice         device_           = VK_NULL_HANDLE;
  VmaAllocator     memory_allocator_ = VK_NULL_HANDLE;
  VkCommandPool    command_pool_     = VK_NULL_HANDLE;
  VkDescriptorPool descriptor_pool_  = VK_NULL_HANDLE;

  Texture                        screen_depth_texture_;
  internal::Swapchain            swapchain_;
  RenderPass                     screen_render_pass_;
  uint32_t                       frame_index_ = 0;
  std::array<internal::Frame, 4> frames_;
  std::vector<Buffer>            buffers_;

public:
  Context() = delete;
  Context(const Desc& desc);

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&& other);
  Context& operator=(Context&& other);

  ~Context();

#include "crystal/common/context_methods.inl"

private:
  friend CommandBuffer;
  friend IndexBuffer;
  friend Library;
  friend Mesh;
  friend Pipeline;
  friend RenderPass;
  friend Texture;
  friend UniformBuffer;
  friend VertexBuffer;

  friend class ::crystal::vulkan::internal::Frame;
  friend class ::crystal::vulkan::internal::Swapchain;

  void add_buffer_(VkBuffer buffer, VmaAllocation allocation) noexcept;
  void retain_buffer_(VkBuffer buffer) noexcept;
  void release_buffer_(VkBuffer buffer) noexcept;
};

inline constexpr uint32_t    Context::screen_width() const { return screen_render_pass_.width(); }
inline constexpr uint32_t    Context::screen_height() const { return screen_render_pass_.height(); }
inline constexpr RenderPass& Context::screen_render_pass() { return screen_render_pass_; }

inline void Context::set_active() {}

inline void Context::wait() { vkDeviceWaitIdle(device_); }

inline Texture Context::create_texture(const TextureDesc& desc) { return Texture(*this, desc); }

inline RenderPass Context::create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures) {
  return RenderPass(*this, color_textures);
}

inline RenderPass Context::create_render_pass(
    const std::initializer_list<std::tuple<const Texture&, AttachmentDesc>> color_textures,
    const std::tuple<const Texture&, AttachmentDesc>                        depth_texture) {
  return RenderPass(*this, color_textures, depth_texture);
}

inline Library Context::create_library(const std::string_view spv_path) {
  return Library(device_, spv_path);
}

inline Pipeline Context::create_pipeline(Library& library, RenderPass& render_pass,
                                         const PipelineDesc& desc) {
  return Pipeline(*this, library, render_pass, desc);
}

inline UniformBuffer Context::create_uniform_buffer(const size_t byte_length) {
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

inline VertexBuffer Context::create_vertex_buffer(const size_t byte_length) {
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

inline IndexBuffer Context::create_index_buffer(const size_t byte_length) {
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
