// Variables that can be defined before including this header:
// - CRYSTAL_IMPL (required)
// - CRYSTAL_IMPL_PROPERTIES
// - CRYSTAL_IMPL_CTOR
// - CRYSTAL_IMPL_METHODS

#include <cstdint>

namespace crystal::CRYSTAL_IMPL {

class Context;
class Mesh;
class Pipeline;
class RenderPass;
class UniformBuffer;

class CommandBuffer {
#ifdef CRYSTAL_IMPL_PROPERTIES
  CRYSTAL_IMPL_PROPERTIES
#endif  // CRYSTAL_IMPL_PROPERTIES

  uint32_t width_;
  uint32_t height_;

public:
  CommandBuffer() = delete;

  CommandBuffer(const CommandBuffer&) = delete;
  CommandBuffer& operator=(const CommandBuffer&) = delete;

  CommandBuffer(CommandBuffer&&) = delete;
  CommandBuffer& operator=(CommandBuffer&&) = delete;

  ~CommandBuffer();

  [[nodiscard]] constexpr uint32_t view_width() const noexcept { return width_; }
  [[nodiscard]] constexpr uint32_t view_height() const noexcept { return height_; }

  void use_render_pass(RenderPass& render_pass);
  void use_pipeline(Pipeline& pipeline);
  void use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t binding);
  void use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t location, uint32_t binding);

  void draw(Mesh& mesh, uint32_t vertex_count, uint32_t instance_count);

private:
  friend class ::crystal::CRYSTAL_IMPL::Context;
  friend class ::crystal::CRYSTAL_IMPL::RenderPass;

#ifdef CRYSTAL_IMPL_CTOR
  CRYSTAL_IMPL_CTOR
#endif  // CRYSTAL_IMPL_CTOR

#ifdef CRYSTAL_IMPL_METHODS
  CRYSTAL_IMPL_METHODS
#endif  // CRYSTAL_IMPL_METHODS
};

}  // namespace crystal::CRYSTAL_IMPL

#undef CRYSTAL_IMPL
#undef CRYSTAL_IMPL_PROPERTIES
#undef CRYSTAL_IMPL_CTOR
#undef CRYSTAL_IMPL_METHODS
