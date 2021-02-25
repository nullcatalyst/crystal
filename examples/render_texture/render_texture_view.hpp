#pragma once

#include "crystal/crystal.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace {

struct Uniform {
  glm::mat4 projection;
  glm::mat4 modelview;
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

}  // namespace

template <typename Ctx>
class RenderTextureView {
  Ctx&                        ctx_;
  typename Ctx::UniformBuffer uniform_buffer_;
  typename Ctx::Texture       depth_texture_;
  typename Ctx::RenderPass    depth_render_pass_;
  typename Ctx::Pipeline      depth_pipeline_;
  typename Ctx::Mesh          cube_mesh_;
  typename Ctx::Pipeline      combine_pipeline_;
  typename Ctx::Mesh          quad_mesh_;

public:
  RenderTextureView(Ctx& ctx, float angle) : ctx_(ctx) {
    const auto    aspect = static_cast<float>(ctx.screen_width()) / ctx.screen_height();
    const Uniform uniform{
        glm::rotate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f)),
    };
    uniform_buffer_ = ctx.create_uniform_buffer(&uniform, sizeof(Uniform));

    auto library = Helpers<Ctx>::create_library(ctx);

    depth_texture_     = ctx.create_texture(crystal::TextureDesc{
        /* .width  = */ 1024,
        /* .height = */ 1024,
        /* .format = */ crystal::TextureFormat::Depth32f,
        /* .sample = */ crystal::TextureSample::Linear,
        /* .repeat = */ crystal::TextureRepeat::RepeatXY,
    });
    depth_render_pass_ = ctx.create_render_pass(
        {}, std::make_tuple(std::ref(depth_texture_), crystal::DepthAttachmentDesc{
                                                          /* .clear = */ true,
                                                          /* .depth = */ 1.0f,
                                                      }));
    depth_pipeline_ =
        ctx.create_pipeline(library, ctx.screen_render_pass(),
                            crystal::PipelineDesc{
                                /* .vertex            = */ Helpers<Ctx>::combine_vertex(),
                                /* .fragment          = */ Helpers<Ctx>::combine_fragment(),
                                /* .cull_mode         = */ crystal::CullMode::Back,
                                /* .winding           = */ crystal::Winding::CounterClockwise,
                                /* .depth_test        = */ crystal::DepthTest::Less,
                                /* .depth_write       = */ crystal::DepthWrite::Enable,
                                /* .blend_src         = */ crystal::AlphaBlend::SrcAlpha,
                                /* .blend_dst         = */ crystal::AlphaBlend::OneMinusSrcAlpha,
                                /* .uniform_bindings  = */
                                {
                                    crystal::UniformBinding{
                                        /* .binding = */ 0,
                                    },
                                },
                                /* .texture_bindings  = */
                                {
                                    crystal::TextureBinding{
                                        /* .binding = */ 0,
                                    },
                                },
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
  }

  ~RenderTextureView() { ctx_.wait(); }

  void frame(float angle) {
    auto cmd = ctx_.next_frame();

    {  // Update uniform buffer.
      const auto aspect =
          static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
      const Uniform uniform{
          Helpers<Ctx>::matrix(aspect) *
              glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(30.0f),
                                      glm::vec3(1.0f, 0.0f, 0.0f)),
                          angle, glm::vec3(0.0f, 1.0f, 0.0f)),

      };
      ctx_.update_uniform_buffer(uniform_buffer_, &uniform, sizeof(Uniform));
    }

    cmd.use_render_pass(depth_render_pass_);
    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(depth_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0, Helpers<Ctx>::uniform_binding());
    cmd.draw(cube_mesh_, 34, 1);

    // cmd.use_render_pass(ctx_.screen_render_pass());
    // cmd.use_pipeline(combine_pipeline_);
    // cmd.use_uniform_buffer(uniform_buffer_, 0, Helpers<Ctx>::uniform_binding());
    // cmd.use_texture(uniform_buffer_, 0, depth_texture_);
    // cmd.draw(quad_mesh_, 4, 1);
  }
};

#if CRYSTAL_USE_OPENGL

namespace {

template <>
struct Helpers<crystal::opengl::Context> {
  static crystal::opengl::Library create_library(crystal::opengl::Context& ctx) {
    return ctx.create_library("examples/render_texture/shaders");
  }

  static const char*     combine_vertex() { return "combine.vert.glsl"; }
  static const char*     combine_fragment() { return "combine.frag.glsl"; }
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

  static const char*     combine_vertex() { return "combine_vert"; }
  static const char*     combine_fragment() { return "combine_frag"; }
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

  static const char*     combine_vertex() { return "combine_vert"; }
  static const char*     combine_fragment() { return "combine_frag"; }
  static const glm::mat4 matrix(float aspect) {
    return glm::orthoRH_ZO(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
  }
  static const uint32_t uniform_binding() { return 1; }
};

}  // namespace

#endif  // ^^^ CRYSTAL_USE_METAL
