#pragma once

#include "crystal/crystal.hpp"
#include "examples/triangle/shaders/shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace {

template <typename Ctx>
static const glm::mat4 matrix(float aspect);

}  // namespace

namespace examples::triangle {

namespace {

constexpr const std::string_view LIBRARY_FILE_NAME = "examples/triangle/shaders/shader.crystallib";

}  // namespace

#if CRYSTAL_HAS_CONCEPTS
template <crystal::concepts::Context Ctx>
#else   // ^^^ CRYSTAL_HAS_CONCEPTS / !CRYSTAL_HAS_CONCEPTS vvv
template <typename Ctx>
#endif  // ^^^ !CRYSTAL_HAS_CONCEPTS
class View {
  Ctx&                        ctx_;
  typename Ctx::UniformBuffer uniform_buffer_;
  typename Ctx::Pipeline      pipeline_;
  typename Ctx::Mesh          mesh_;

public:
  View(Ctx& ctx, float angle) : ctx_(ctx) {
    // const auto             aspect = static_cast<float>(ctx.screen_width()) / ctx.screen_height();
    // const shaders::Uniform uniform{
    //     glm::rotate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), angle, glm::vec3(0.0f,
    //     0.0f, 1.0f)),
    // };
    // uniform_buffer_ = ctx.create_uniform_buffer(&uniform, sizeof(shaders::Uniform));

    auto library = ctx.create_library(LIBRARY_FILE_NAME);
    pipeline_    = ctx.create_pipeline(library, ctx.screen_render_pass(), shaders::triangle_desc);

    auto vertex_buffer = ctx.create_vertex_buffer({
        // clang-format off
        shaders::Vertex{glm::vec4{0.75f * -0.866f, 0.75f * -0.5f, 0.0f, 1.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        shaders::Vertex{glm::vec4{0.75f *  0.866f, 0.75f * -0.5f, 0.0f, 1.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        shaders::Vertex{glm::vec4{0.75f *  0.000f, 0.75f *  1.0f, 0.0f, 1.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        // clang-format on
    });
    mesh_ = ctx.create_mesh({
        std::make_tuple(0, std::ref(vertex_buffer)),
    });
  }

  ~View() { ctx_.wait(); }

  void frame(float angle) {
    auto cmd = ctx_.next_frame();

    ctx_.set_clear_color(
        ctx_.screen_render_pass(), 0,
        crystal::ClearValue{.color = {0.0f, 0.5f * std::sin(angle) * std::sin(angle), 0.0f, 0.0f}});

    // {  // Update uniform buffer.
    //   const auto aspect =
    //       static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
    //   const shaders::Uniform uniform{
    //       glm::rotate(matrix<Ctx>(aspect), angle, glm::vec3{0.0f, 0.0f, 1.0f}),
    //   };
    //   ctx_.update_uniform_buffer(uniform_buffer_, &uniform, sizeof(shaders::Uniform));
    // }

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(pipeline_);
    // cmd.use_uniform_buffer(uniform_buffer_, 0, 0);
    cmd.draw(mesh_, 3, 1);
  }
};

}  // namespace examples::triangle

#if CRYSTAL_USE_OPENGL

namespace {

template <>
static const glm::mat4 matrix<crystal::opengl::Context>(float aspect) {
  return glm::ortho(-aspect, aspect, -1.0f, 1.0f);
}

}  // namespace

#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN

namespace {

static const glm::mat4 matrix<crystal::vulkan::Context>(float aspect) {
  return glm::ortho(-aspect, aspect, 1.0f, -1.0f);
}

}  // namespace

#endif  // ^^^ CRYSTAL_USE_VULKAN

#if CRYSTAL_USE_METAL

namespace {

template <>
static const glm::mat4 matrix<crystal::metal::Context>(float aspect) {
  return glm::ortho(-aspect, aspect, -1.0f, 1.0f);
}

}  // namespace

#endif  // ^^^ CRYSTAL_USE_METAL
