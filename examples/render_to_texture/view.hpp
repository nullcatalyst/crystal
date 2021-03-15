#pragma once

#include "crystal/crystal.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace examples::render_to_texture {

namespace {

struct Uniform {
  glm::mat4 cube_matrix;
  glm::mat4 quad_matrix;
};

struct CubeVertex {
  glm::vec4 position;
  glm::vec4 color;
};

struct QuadVertex {
  glm::vec4 position;
  glm::vec4 tex_coord;
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
      /* .cube_matrix = */ Helpers<Ctx>::matrix(1.0f) *
          glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(30.0f),
                                  glm::vec3(1.0f, 0.0f, 0.0f)),
                      angle, glm::vec3(0.0f, 1.0f, 0.0f)),
      /* .quad_matrix = */ Helpers<Ctx>::matrix(aspect) *
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
  typename Ctx::Texture       cube_texture_;
  typename Ctx::RenderPass    cube_render_pass_;
  typename Ctx::Pipeline      cube_pipeline_;
  typename Ctx::Mesh          cube_mesh_;
  typename Ctx::Pipeline      quad_pipeline_;
  typename Ctx::Mesh          quad_mesh_;

public:
  RenderTextureView(Ctx& ctx, float angle) : ctx_(ctx) {
    const Uniform uniform = create_uniform(ctx, angle);
    uniform_buffer_       = ctx.create_uniform_buffer(&uniform, sizeof(Uniform));

    auto library = Helpers<Ctx>::create_library(ctx);

    cube_texture_     = ctx.create_texture(crystal::TextureDesc{
        /* .width  = */ 1024,
        /* .height = */ 1024,
        /* .format = */ crystal::TextureFormat::RGBA8u,
        /* .sample = */ crystal::TextureSample::Linear,
        /* .repeat = */ crystal::TextureRepeat::RepeatXY,
    });
    cube_render_pass_ = ctx.create_render_pass({
        std::make_tuple(std::ref(cube_texture_),
                        crystal::AttachmentDesc{
                            .clear = true,
                            .clear_value =
                                {
                                    .color = {
                                        /* .red   = */ 0.5f,
                                        /* .green = */ 0.5f,
                                        /* .blue  = */ 0.5f,
                                        /* .alpha = */ 1.0f,
                                    },
                                },
                        }),
    });
    cube_pipeline_ =
        // ctx.create_pipeline(library, ctx.screen_render_pass(),
        ctx.create_pipeline(library, cube_render_pass_,
                            crystal::PipelineDesc{
                                /* .vertex            = */ Helpers<Ctx>::cube_vertex(),
                                /* .fragment          = */ Helpers<Ctx>::cube_fragment(),
                                /* .cull_mode         = */ crystal::CullMode::Back,
                                /* .winding           = */ crystal::Winding::CounterClockwise,
                                /* .depth_test        = */ crystal::DepthTest::Always,
                                /* .depth_write       = */ crystal::DepthWrite::Disable,
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
                                        /* .offset       = */ offsetof(CubeVertex, position),
                                        /* .buffer_index = */ 0,
                                    },
                                    crystal::VertexAttributeDesc{
                                        /* .attribute    = */ 1,
                                        /* .offset       = */ offsetof(CubeVertex, color),
                                        /* .buffer_index = */ 0,
                                    },
                                },
                                /* .vertex_buffers    = */
                                {
                                    crystal::VertexBufferDesc{
                                        /* .buffer_index  = */ 0,
                                        /* .stride        = */ sizeof(CubeVertex),
                                        /*. step_function = */ crystal::StepFunction::PerVertex,
                                    },
                                },
                            });

    quad_pipeline_ =
        ctx.create_pipeline(library, ctx.screen_render_pass(),
                            crystal::PipelineDesc{
                                /* .vertex            = */ Helpers<Ctx>::quad_vertex(),
                                /* .fragment          = */ Helpers<Ctx>::quad_fragment(),
                                /* .cull_mode         = */ crystal::CullMode::None,
                                /* .winding           = */ crystal::Winding::CounterClockwise,
                                /* .depth_test        = */ crystal::DepthTest::Always,
                                /* .depth_write       = */ crystal::DepthWrite::Disable,
                                /* .blend_src         = */ crystal::AlphaBlend::One,
                                /* .blend_dst         = */ crystal::AlphaBlend::Zero,
                                /* .uniform_bindings  = */
                                {
                                    crystal::UniformBinding{/* .binding = */ 0},
                                },
                                /* .texture_bindings  = */
                                {
                                    crystal::TextureBinding{/* .binding = */ 0},
                                },
                                /* .vertex_attributes = */
                                {
                                    crystal::VertexAttributeDesc{
                                        /* .attribute    = */ 0,
                                        /* .offset       = */ offsetof(QuadVertex, position),
                                        /* .buffer_index = */ 0,
                                    },
                                    crystal::VertexAttributeDesc{
                                        /* .attribute    = */ 1,
                                        /* .offset       = */ offsetof(QuadVertex, tex_coord),
                                        /* .buffer_index = */ 0,
                                    },
                                },
                                /* .vertex_buffers    = */
                                {
                                    crystal::VertexBufferDesc{
                                        /* .buffer_index  = */ 0,
                                        /* .stride        = */ sizeof(QuadVertex),
                                        /*. step_function = */ crystal::StepFunction::PerVertex,
                                    },
                                },
                            });

    const std::array<CubeVertex, 24> cube_vertices{
        // clang-format off
        // -x
        CubeVertex{glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
        // +x
        CubeVertex{glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        // -y
        CubeVertex{glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)},
        // +y
        CubeVertex{glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        // -z
        CubeVertex{glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        // +z
        CubeVertex{glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        CubeVertex{glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        // clang-format on
    };
    const std::array<uint16_t, 34> cube_indices{
        // clang-format off
             0,  1,  2,  3,  3,
         4,  4,  5,  6,  7,  7,
         8,  8,  9, 10, 11, 11,
        12, 12, 13, 14, 15, 15,
        16, 16, 17, 18, 19, 19,
        20, 20, 21, 22, 23,
        // clang-format on
    };
    auto cube_vertex_buffer =
        ctx.create_vertex_buffer(cube_vertices.data(), sizeof(CubeVertex) * cube_vertices.size());
    auto cube_index_buffer =
        ctx.create_index_buffer(cube_indices.data(), sizeof(uint16_t) * cube_indices.size());
    cube_mesh_ = ctx.create_mesh(
        {
            std::make_tuple(0, std::ref(cube_vertex_buffer)),
        },
        cube_index_buffer);

    const std::array<QuadVertex, 4> quad_vertices{
        // clang-format off
        QuadVertex{glm::vec4(-0.75f, -0.75f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        QuadVertex{glm::vec4( 0.75f, -0.75f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        QuadVertex{glm::vec4(-0.75f,  0.75f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)},
        QuadVertex{glm::vec4( 0.75f,  0.75f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        // clang-format on
    };
    auto quad_vertex_buffer =
        ctx.create_vertex_buffer(quad_vertices.data(), sizeof(QuadVertex) * quad_vertices.size());
    quad_mesh_ = ctx.create_mesh({
        std::make_tuple(0, std::ref(quad_vertex_buffer)),
    });
  }

  ~RenderTextureView() { ctx_.wait(); }

  void frame(float angle) {
    auto cmd = ctx_.next_frame();

    {  // Update uniform buffer.
      const Uniform uniform = create_uniform(ctx_, angle);
      ctx_.update_uniform_buffer(uniform_buffer_, &uniform, sizeof(Uniform));
    }

    cmd.use_render_pass(cube_render_pass_);
    cmd.use_pipeline(cube_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0, Helpers<Ctx>::uniform_binding());
    cmd.draw(cube_mesh_, 34, 1);

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(quad_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0, Helpers<Ctx>::uniform_binding());
    cmd.use_texture(cube_texture_, 0, 0);
    cmd.draw(quad_mesh_, 4, 1);
  }
};

#if CRYSTAL_USE_OPENGL

namespace {

template <>
struct Helpers<crystal::opengl::Context> {
  static crystal::opengl::Library create_library(crystal::opengl::Context& ctx) {
    return ctx.create_library("examples/render_texture/shaders");
  }

  static const char*     cube_vertex() { return "cube.vert.glsl"; }
  static const char*     cube_fragment() { return "cube.frag.glsl"; }
  static const char*     quad_vertex() { return "quad.vert.glsl"; }
  static const char*     quad_fragment() { return "quad.frag.glsl"; }
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
    return ctx.create_library("examples/render_texture/shaders/shader.spv");
  }

  static const char*     cube_vertex() { return "cube_vert"; }
  static const char*     cube_fragment() { return "cube_frag"; }
  static const char*     quad_vertex() { return "quad_vert"; }
  static const char*     quad_fragment() { return "quad_frag"; }
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
    return ctx.create_library("examples/render_texture/shaders/shader.metallib");
  }

  static const char*     cube_vertex() { return "cube_vert"; }
  static const char*     cube_fragment() { return "cube_frag"; }
  static const char*     quad_vertex() { return "quad_vert"; }
  static const char*     quad_fragment() { return "quad_frag"; }
  static const glm::mat4 matrix(float aspect) {
    return glm::orthoRH_ZO(-aspect, aspect, 1.0f, -1.0f, -1.0f, 1.0f);
  }
  static const uint32_t uniform_binding() { return 1; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_METAL

}  // namespace examples::render_to_texture
