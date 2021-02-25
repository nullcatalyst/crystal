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

}  // namespace

template <typename Ctx>
class TriangleView {
  Ctx&                        ctx_;
  typename Ctx::UniformBuffer uniform_buffer_;
  typename Ctx::Pipeline      pipeline_;
  typename Ctx::Mesh          mesh_;

public:
  TriangleView(Ctx& ctx, float angle) : ctx_(ctx) {
    const auto    aspect = static_cast<float>(ctx.screen_width()) / ctx.screen_height();
    const Uniform uniform{
        glm::rotate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f)),
    };
    uniform_buffer_ = ctx.create_uniform_buffer(&uniform, sizeof(Uniform));

    auto library = Helpers<Ctx>::create_library(ctx);
    pipeline_ =
        ctx.create_pipeline(library, ctx.screen_render_pass(),
                            crystal::PipelineDesc{
                                /* .vertex            = */ Helpers<Ctx>::vertex(),
                                /* .fragment          = */ Helpers<Ctx>::fragment(),
                                /* .cull_mode         = */ crystal::CullMode::None,
                                /* .winding           = */ crystal::Winding::CounterClockwise,
                                /* .depth_test        = */ crystal::DepthTest::Always,
                                /* .depth_write       = */ crystal::DepthWrite::Disable,
                                /* .blend_src         = */ crystal::AlphaBlend::SrcAlpha,
                                /* .blend_dst         = */ crystal::AlphaBlend::OneMinusSrcAlpha,
                                /* .uniform_bindings  = */
                                {
                                    crystal::UniformBinding{
                                        /* .binding = */ 0,
                                    },
                                },
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

    const std::array<Vertex, 3> vertices{
        // clang-format off
        Vertex{glm::vec4(0.75f * -0.866f, 0.75f * -0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(0.75f *  0.866f, 0.75f * -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(0.75f *  0.000f, 0.75f *  1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        // clang-format on
    };
    auto vertex_buffer =
        ctx.create_vertex_buffer(vertices.data(), sizeof(Vertex) * vertices.size());
    mesh_ = ctx.create_mesh({
        std::make_tuple(0, std::ref(vertex_buffer)),
    });
  }

  ~TriangleView() { ctx_.wait(); }

  void frame(float angle) {
    auto cmd = ctx_.next_frame();

    {  // Update uniform buffer.
      const auto aspect =
          static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
      const Uniform uniform{
          glm::rotate(Helpers<Ctx>::matrix(aspect), angle, glm::vec3(0.0f, 0.0f, 1.0f)),
      };
      ctx_.update_uniform_buffer(uniform_buffer_, &uniform, sizeof(Uniform));
    }

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0, Helpers<Ctx>::uniform_binding());
    cmd.draw(mesh_, 3, 1);
  }
};

#if CRYSTAL_USE_OPENGL

namespace {

template <>
struct Helpers<crystal::opengl::Context> {
  static crystal::opengl::Library create_library(crystal::opengl::Context& ctx) {
    return ctx.create_library("examples/triangle/shaders");
  }

  static const char*     vertex() { return "shader.vert.glsl"; }
  static const char*     fragment() { return "shader.frag.glsl"; }
  static const glm::mat4 matrix(float aspect) { return glm::ortho(-aspect, aspect, -1.0f, 1.0f); }
  static const uint32_t  uniform_binding() { return 0; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_OPENGL

#if CRYSTAL_USE_VULKAN

namespace {

template <>
struct Helpers<crystal::vulkan::Context> {
  static crystal::vulkan::Library create_library(crystal::vulkan::Context& ctx) {
    return ctx.create_library("examples/triangle/shaders/shader.spv");
  }

  static const char*     vertex() { return "triangle_vert"; }
  static const char*     fragment() { return "triangle_frag"; }
  static const glm::mat4 matrix(float aspect) { return glm::ortho(-aspect, aspect, 1.0f, -1.0f); }
  static const uint32_t  uniform_binding() { return 0; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_VULKAN

#if CRYSTAL_USE_METAL

namespace {

template <>
struct Helpers<crystal::metal::Context> {
  static crystal::metal::Library create_library(crystal::metal::Context& ctx) {
    return ctx.create_library("examples/triangle/shaders/shader.metallib");
  }

  static const char*     vertex() { return "triangle_vert"; }
  static const char*     fragment() { return "triangle_frag"; }
  static const glm::mat4 matrix(float aspect) { return glm::ortho(-aspect, aspect, -1.0f, 1.0f); }
  static const uint32_t  uniform_binding() { return 1; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_METAL
