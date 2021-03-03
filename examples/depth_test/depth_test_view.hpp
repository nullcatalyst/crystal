#pragma once

#include "crystal/crystal.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace {

struct Uniform {
  glm::mat4 matrix;
};

struct Vertex {
  glm::vec4 position;
  glm::vec4 color;
};

template <typename Ctx>
struct Helpers {
  static typename Ctx::Library create_library(Ctx& ctx);
  static const char*           vertex();
  static const char*           fragment();
  static const glm::mat4       matrix(float aspect);
  static const uint32_t        uniform_binding();
};

template <typename Ctx>
constexpr Uniform create_uniform(Ctx& ctx, float angle) {
  const auto aspect =
      static_cast<float>(ctx.screen_width()) / static_cast<float>(ctx.screen_height());

  return Uniform{
      /* .matrix = */ Helpers<Ctx>::matrix(aspect) *
          glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(30.0f),
                                  glm::vec3(1.0f, 0.0f, 0.0f)),
                      angle, glm::vec3(0.0f, 1.0f, 0.0f)),
  };
}

}  // namespace

#if CRYSTAL_HAS_CONCEPTS
template <crystal::concepts::Context Ctx>
#else   // ^^^ CRYSTAL_HAS_CONCEPTS / !CRYSTAL_HAS_CONCEPTS vvv
template <typename Ctx>
#endif  // ^^^ !CRYSTAL_HAS_CONCEPTS
class RenderTextureView {
  Ctx&                        ctx_;
  typename Ctx::UniformBuffer uniform_buffer_;
  typename Ctx::RenderPass    render_pass_;
  typename Ctx::Pipeline      pipeline_;
  typename Ctx::Mesh          mesh_;

public:
  RenderTextureView(Ctx& ctx, float angle) : ctx_(ctx) {
    const Uniform uniform = create_uniform(ctx, angle);
    uniform_buffer_       = ctx.create_uniform_buffer(&uniform, sizeof(Uniform));

    auto library = Helpers<Ctx>::create_library(ctx);
    pipeline_ =
        ctx.create_pipeline(library, ctx.screen_render_pass(),
                            crystal::PipelineDesc{
                                /* .vertex            = */ Helpers<Ctx>::depth_test_vertex(),
                                /* .fragment          = */ Helpers<Ctx>::depth_test_fragment(),
                                /* .cull_mode         = */ crystal::CullMode::None,
                                /* .winding           = */ crystal::Winding::CounterClockwise,
                                /* .depth_test        = */ crystal::DepthTest::Less,
                                /* .depth_write       = */ crystal::DepthWrite::Enable,
                                /* .blend_src         = */ crystal::AlphaBlend::One,
                                /* .blend_dst         = */ crystal::AlphaBlend::Zero,
                                /* .uniform_bindings  = */
                                {
                                    crystal::UniformBinding{/* .binding = */ 0},
                                },
                                /* .texture_bindings  = */ {},
                                /* .vertex_attributes = */
                                {
                                    crystal::VertexAttributeDesc{
                                        /* .attribute    = */ 0,
                                        /* .offset       = */ offsetof(Vertex, position),
                                        /* .buffer_index = */ 0,
                                    },
                                    crystal::VertexAttributeDesc{
                                        /* .attribute    = */ 1,
                                        /* .offset       = */ offsetof(Vertex, color),
                                        /* .buffer_index = */ 0,
                                    },
                                },
                                /* .vertex_buffers    = */
                                {
                                    crystal::VertexBufferDesc{
                                        /* .buffer_index  = */ 0,
                                        /* .stride        = */ sizeof(Vertex),
                                        /*. step_function = */ crystal::StepFunction::PerVertex,
                                    },
                                },
                            });

    const std::array<Vertex, 8> vertices{
        // clang-format off
        // -z
        Vertex{glm::vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec4{-0.5f, -0.5f, -0.5f, 1.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec4{-0.5f,  0.5f, -0.5f, 1.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        // +z
        Vertex{glm::vec4{-0.5f, -0.5f,  0.5f, 1.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec4{-0.5f,  0.5f,  0.5f, 1.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        Vertex{glm::vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        // clang-format on
    };
    const std::array<uint16_t, 10> indices{
        // clang-format off
             0,  1,  2,  3,  3,
         4,  4,  5,  6,  7,
        // clang-format on
    };
    auto vertex_buffer =
        ctx.create_vertex_buffer(vertices.data(), sizeof(Vertex) * vertices.size());
    auto index_buffer = ctx.create_index_buffer(indices.data(), sizeof(uint16_t) * indices.size());
    mesh_             = ctx.create_mesh(
        {
            std::make_tuple(0, std::ref(vertex_buffer)),
        },
        index_buffer);
  }

  ~RenderTextureView() { ctx_.wait(); }

  void frame(float angle) {
    auto cmd = ctx_.next_frame();

    {  // Update uniform buffer.
      const Uniform uniform = create_uniform(ctx_, angle);
      ctx_.update_uniform_buffer(uniform_buffer_, &uniform, sizeof(Uniform));
    }

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0, Helpers<Ctx>::uniform_binding());
    cmd.draw(mesh_, 10, 1);
  }
};

#if CRYSTAL_USE_OPENGL

namespace {

template <>
struct Helpers<crystal::opengl::Context> {
  static crystal::opengl::Library create_library(crystal::opengl::Context& ctx) {
    return ctx.create_library("examples/depth_test/shaders");
  }

  static const char*     depth_test_vertex() { return "depth_test.vert.glsl"; }
  static const char*     depth_test_fragment() { return "depth_test.frag.glsl"; }
  static const glm::mat4 matrix(float aspect) {
    return glm::orthoRH_NO(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
  }
  static const uint32_t uniform_binding() { return 0; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN

namespace {

template <>
struct Helpers<crystal::vulkan::Context> {
  static crystal::vulkan::Library create_library(crystal::vulkan::Context& ctx) {
    return ctx.create_library("examples/depth_test/shaders/shader.spv");
  }

  static const char*     depth_test_vertex() { return "depth_test_vert"; }
  static const char*     depth_test_fragment() { return "depth_test_frag"; }
  static const glm::mat4 matrix(float aspect) {
    return glm::orthoRH_ZO(-aspect, aspect, 1.0f, -1.0f, -1.0f, 1.0f);
  }
  static const uint32_t uniform_binding() { return 0; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_VULKAN

#if CRYSTAL_USE_METAL

namespace {

template <>
struct Helpers<crystal::metal::Context> {
  static crystal::metal::Library create_library(crystal::metal::Context& ctx) {
    return ctx.create_library("examples/depth_test/shaders/shader.metallib");
  }

  static const char*     depth_test_vertex() { return "depth_test_vert"; }
  static const char*     depth_test_fragment() { return "depth_test_frag"; }
  static const glm::mat4 matrix(float aspect) {
    // return glm::orthoRH_ZO(-aspect, aspect, 1.0f, -1.0f, -1.0f, 1.0f);
    return glm::translate(glm::perspectiveRH_ZO(glm::radians(60.0f), aspect, 1.0f, 4.0f),
                          glm::vec3{0.0f, 0.0f, -2.0f});
  }
  static const uint32_t uniform_binding() { return 1; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_METAL
