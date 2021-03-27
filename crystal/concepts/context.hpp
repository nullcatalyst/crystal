#pragma once

#include <concepts>
#include <cstdint>
#include <string_view>
#include <utility>

#include "crystal/common/pipeline_desc.hpp"
#include "crystal/common/render_pass_desc.hpp"
#include "crystal/common/texture_desc.hpp"

namespace crystal::concepts {

template <typename T>
concept Context = requires(T& t, const T& ct) {
  typename T::CommandBuffer;
  typename T::IndexBuffer;
  typename T::Library;
  typename T::Mesh;
  typename T::Pipeline;
  typename T::RenderPass;
  typename T::Texture;
  typename T::UniformBuffer;
  typename T::VertexBuffer;

  { ct.screen_width() }
  ->std::same_as<uint32_t>;

  { ct.screen_height() }
  ->std::same_as<uint32_t>;

  { t.screen_render_pass() }
  ->std::same_as<typename T::RenderPass&>;

  { t.change_resolution(std::declval<uint32_t>(), std::declval<uint32_t>()) }
  ->std::same_as<void>;

  { t.wait() }
  ->std::same_as<void>;

  { t.next_frame() }
  ->std::same_as<typename T::CommandBuffer>;

  { t.create_texture(std::declval<const TextureDesc&>()) }
  ->std::same_as<typename T::Texture>;

  {
    t.create_render_pass(std::declval<const std::initializer_list<
                             std::tuple<const typename T::Texture&, ColorAttachmentDesc>>>())
  }
  ->std::same_as<typename T::RenderPass>;

  {
    t.create_render_pass(
        std::declval<const std::initializer_list<
            std::tuple<const typename T::Texture&, ColorAttachmentDesc>>>(),
        std::declval<const std::tuple<const typename T::Texture&, DepthAttachmentDesc>>())
  }
  ->std::same_as<typename T::RenderPass>;

  { t.create_library(std::declval<const std::string_view>()) }
  ->std::same_as<typename T::Library>;

  {
    t.create_pipeline(std::declval<typename T::Library&>(), std::declval<typename T::RenderPass&>(),
                      std::declval<const PipelineDesc&>())
  }
  ->std::same_as<typename T::Pipeline>;

  { t.create_uniform_buffer(std::declval<size_t>()) }
  ->std::same_as<typename T::UniformBuffer>;

  { t.create_uniform_buffer(std::declval<const void* const>(), std::declval<const size_t>()) }
  ->std::same_as<typename T::UniformBuffer>;

  {
    t.update_uniform_buffer(std::declval<typename T::UniformBuffer&>(),
                            std::declval<const void* const>(), std::declval<const size_t>())
  }
  ->std::same_as<void>;

  { t.create_vertex_buffer(std::declval<size_t>()) }
  ->std::same_as<typename T::VertexBuffer>;

  { t.create_vertex_buffer(std::declval<const void* const>(), std::declval<const size_t>()) }
  ->std::same_as<typename T::VertexBuffer>;

  {
    t.update_vertex_buffer(std::declval<typename T::VertexBuffer&>(),
                           std::declval<const void* const>(), std::declval<const size_t>())
  }
  ->std::same_as<void>;

  { t.create_index_buffer(std::declval<const size_t>()) }
  ->std::same_as<typename T::IndexBuffer>;

  { t.create_index_buffer(std::declval<const uint16_t* const>(), std::declval<const size_t>()) }
  ->std::same_as<typename T::IndexBuffer>;

  {
    t.update_index_buffer(std::declval<typename T::IndexBuffer&>(),
                          std::declval<const uint16_t* const>(), std::declval<const size_t>())
  }
  ->std::same_as<void>;

  {
    t.create_mesh(
        std::declval<
            const std::initializer_list<std::tuple<uint32_t, const typename T::VertexBuffer&>>>())
  }
  ->std::same_as<typename T::Mesh>;

  {
    t.create_mesh(
        std::declval<
            const std::initializer_list<std::tuple<uint32_t, const typename T::VertexBuffer&>>>(),
        std::declval<typename T::IndexBuffer&>())
  }
  ->std::same_as<typename T::Mesh>;
};

}  // namespace crystal::concepts
