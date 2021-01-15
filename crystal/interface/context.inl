#include <cstddef>
#include <initializer_list>
#include <string_view>
#include <tuple>

#include "crystal/common/pipeline_desc.hpp"
#include "crystal/common/render_pass_desc.hpp"
#include "util/unsafe/deferred_ctor.hpp"

// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

#ifndef CRYSTAL_IMPL
#error CRYSTAL_IMPL must be defined with the name of the implementation
#endif  // CRYSTAL_IMPL

namespace crystal::CRYSTAL_IMPL {

class Context {
public:
  using CommandBuffer  = ::crystal::CRYSTAL_IMPL::CommandBuffer;
  using Library        = ::crystal::CRYSTAL_IMPL::Library;
  using IndexBuffer    = ::crystal::CRYSTAL_IMPL::IndexBuffer;
  using Mesh           = ::crystal::CRYSTAL_IMPL::Mesh;
  using PipelineDesc   = ::crystal::PipelineDesc;
  using Pipeline       = ::crystal::CRYSTAL_IMPL::Pipeline;
  using RenderPassDesc = ::crystal::RenderPassDesc;
  using RenderPass     = ::crystal::CRYSTAL_IMPL::RenderPass;
  using Texture        = ::crystal::CRYSTAL_IMPL::Texture;
  using UniformBuffer  = ::crystal::CRYSTAL_IMPL::UniformBuffer;
  using VertexBuffer   = ::crystal::CRYSTAL_IMPL::VertexBuffer;

private:
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

  ::util::unsafe::DeferredCtor<RenderPass> screen_render_pass_;
  uint32_t                                 width_  = 0;
  uint32_t                                 height_ = 0;

public:
#ifdef CRYSTAL_IMPL_CTOR
  CRYSTAL_IMPL_CTOR
#endif  // CRYSTAL_IMPL_CTOR

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&&) = default;
  Context& operator=(Context&&) = default;

  ~Context();

  [[nodiscard]] constexpr uint32_t view_width() const { return width_; }
  [[nodiscard]] constexpr uint32_t view_height() const { return height_; }

  constexpr RenderPass& screen_render_pass() { return screen_render_pass_; }

  void change_resolution(uint32_t width, uint32_t height);

  void wait();

  CommandBuffer next_frame();

  RenderPass create_render_pass(const RenderPassDesc& desc);

  Library  create_library(const std::string_view base_path);
  Pipeline create_pipeline(Library& library, RenderPass& render_pass, const PipelineDesc& desc);

  UniformBuffer create_uniform_buffer(size_t byte_length);
  UniformBuffer create_uniform_buffer(const void* const data_ptr, const size_t byte_length);
  void          update_uniform_buffer(UniformBuffer& uniform_buffer, const void* const data_ptr,
                                      const size_t byte_length);

  VertexBuffer create_vertex_buffer(size_t byte_length);
  VertexBuffer create_vertex_buffer(const void* const data_ptr, const size_t byte_length);
  void         update_vertex_buffer(VertexBuffer& vertex_buffer, const void* const data_ptr,
                                    const size_t byte_length);

  IndexBuffer create_index_buffer(size_t byte_length);
  IndexBuffer create_index_buffer(const uint16_t* const data_ptr, const size_t byte_length);
  void        update_index_buffer(IndexBuffer& vertex_buffer, const uint16_t* const data_ptr,
                                  const size_t byte_length);

  Mesh create_mesh(const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings);
  Mesh create_mesh(const std::initializer_list<std::tuple<uint32_t, const VertexBuffer&>> bindings,
                   const IndexBuffer& index_buffer);

private:
  friend class ::crystal::CRYSTAL_IMPL::CommandBuffer;
  friend class ::crystal::CRYSTAL_IMPL::Library;
  friend class ::crystal::CRYSTAL_IMPL::IndexBuffer;
  friend class ::crystal::CRYSTAL_IMPL::Mesh;
  friend class ::crystal::CRYSTAL_IMPL::Pipeline;
  friend class ::crystal::CRYSTAL_IMPL::RenderPass;
  friend class ::crystal::CRYSTAL_IMPL::Texture;
  friend class ::crystal::CRYSTAL_IMPL::UniformBuffer;
  friend class ::crystal::CRYSTAL_IMPL::VertexBuffer;

#ifdef CRYSTAL_IMPL_METHODS
  CRYSTAL_IMPL_METHODS
#endif  // CRYSTAL_IMPL_METHODS
};

}  // namespace crystal::CRYSTAL_IMPL

#undef CRYSTAL_IMPL
#undef CRYSTAL_IMPL_PROPERTIES
#undef CRYSTAL_IMPL_CTOR
#undef CRYSTAL_IMPL_METHODS
