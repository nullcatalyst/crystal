#pragma once

#include "crystal/crystal.hpp"
#include "examples/render_to_texture/shader.hpp"
#include "examples/render_to_texture/state.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace examples::render_to_texture {

Uniform create_uniform(float aspect, State state) {
  return Uniform{
      /* .cube_matrix = */ glm::orthoRH_NO(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f) *
          glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(-30.0f),
                                  glm::vec3(1.0f, 0.0f, 0.0f)),
                      state.cube_angle, glm::vec3(0.0f, 1.0f, 0.0f)),
      /* .quad_matrix = */ glm::orthoRH_NO(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f) *
          glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(-30.0f),
                                  glm::vec3(1.0f, 0.0f, 0.0f)),
                      state.quad_angle, glm::vec3(0.0f, 1.0f, 0.0f)),
  };
}

class View {
public:
  virtual ~View() = default;

  virtual void frame(const State& state) = 0;
};

#if CRYSTAL_HAS_CONCEPTS
template <crystal::concepts::Context Ctx>
#else   // ^^^ CRYSTAL_HAS_CONCEPTS / !CRYSTAL_HAS_CONCEPTS vvv
template <typename Ctx>
#endif  // ^^^ !CRYSTAL_HAS_CONCEPTS
class ViewImpl : public View {
  Ctx&                        ctx_;
  typename Ctx::UniformBuffer uniform_buffer_;
  typename Ctx::Texture       cube_texture_;
  typename Ctx::RenderPass    cube_render_pass_;
  typename Ctx::Pipeline      cube_pipeline_;
  typename Ctx::Mesh          cube_mesh_;
  typename Ctx::Pipeline      quad_pipeline_;
  typename Ctx::Mesh          quad_mesh_;

public:
  ViewImpl(Ctx& ctx, const State& state) : ctx_(ctx) {
    const auto aspect = static_cast<float>(ctx_.screen_width()) / ctx_.screen_height();
    uniform_buffer_   = ctx.create_uniform_buffer(create_uniform(aspect, state));

    auto library = ctx.create_library("examples/render_to_texture/shader.crystallib");

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
    cube_pipeline_    = ctx.create_pipeline(library, cube_render_pass_, cube_desc);

    quad_pipeline_ = ctx.create_pipeline(library, ctx.screen_render_pass(), quad_desc);

    auto cube_vertex_buffer = ctx.create_vertex_buffer({
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
    });
    auto cube_index_buffer = ctx.create_index_buffer({
        // clang-format off
             0,  1,  2,  3,  3,
         4,  4,  5,  6,  7,  7,
         8,  8,  9, 10, 11, 11,
        12, 12, 13, 14, 15, 15,
        16, 16, 17, 18, 19, 19,
        20, 20, 21, 22, 23,
        // clang-format on
    });
    cube_mesh_ = ctx.create_mesh(
        {
            std::make_tuple(0, std::ref(cube_vertex_buffer)),
        },
        cube_index_buffer);

    auto quad_vertex_buffer = ctx.create_vertex_buffer({
        // clang-format off
        QuadVertex{glm::vec4(-0.75f, -0.75f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        QuadVertex{glm::vec4( 0.75f, -0.75f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        QuadVertex{glm::vec4(-0.75f,  0.75f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)},
        QuadVertex{glm::vec4( 0.75f,  0.75f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        // clang-format on
    });
    quad_mesh_ = ctx.create_mesh({
        std::make_tuple(0, std::ref(quad_vertex_buffer)),
    });
  }

  ~ViewImpl() { ctx_.wait(); }

  virtual void frame(const State& state) override {
    auto cmd = ctx_.next_frame();

    {  // Update uniform buffer.
      const auto aspect =
          static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
      ctx_.update_uniform_buffer(uniform_buffer_, create_uniform(aspect, state));
    }

    cmd.use_render_pass(cube_render_pass_);
    cmd.use_pipeline(cube_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 1);
    cmd.draw(cube_mesh_, 34, 1);

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(quad_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0);
    cmd.use_texture(cube_texture_, 0);
    cmd.draw(quad_mesh_, 4, 1);
  }
};

}  // namespace examples::render_to_texture
