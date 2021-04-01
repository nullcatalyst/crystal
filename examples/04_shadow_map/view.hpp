#pragma once

#include "crystal/crystal.hpp"
#include "examples/04_shadow_map/shader.hpp"
#include "examples/04_shadow_map/state.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace examples::shadow_map {

Uniform create_uniform(float aspect, State state) {
  return Uniform{
      /* .shadow_matrix     = */ glm::orthoRH_ZO(-5.0f, 5.0f, -5.0f, 5.0f, -6.0f, 6.0f) *
          glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(15.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f)),
                      glm::radians(75.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
      /* .projection_matrix = */ glm::perspectiveRH_ZO(glm::radians(60.0f), aspect, 0.01f, 10.0f) *
          glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -2.0f)),
      /* .rotation_matrix   = */
      glm::rotate(
          glm::rotate(glm::identity<glm::mat4>(), state.pitch_angle, glm::vec3(1.0f, 0.0f, 0.0f)),
          state.yaw_angle, glm::vec3(0.0f, 1.0f, 0.0f)),
      /* .orbit_matrix      = */
      glm::rotate(
          glm::rotate(glm::identity<glm::mat4>(), glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
          state.orbit_angle, glm::vec3(0.0f, 1.0f, 0.0f)),
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
  typename Ctx::Texture       shadow_texture_;
  typename Ctx::RenderPass    shadow_render_pass_;
  typename Ctx::Pipeline      shadow_pipeline_;
  typename Ctx::Pipeline      combine_pipeline_;
  typename Ctx::Mesh          cube_mesh_;
  typename Ctx::Mesh          floor_mesh_;

public:
  ViewImpl(Ctx& ctx, const State& state) : ctx_(ctx) {
    const auto aspect = static_cast<float>(ctx_.screen_width()) / ctx_.screen_height();
    uniform_buffer_   = ctx.create_uniform_buffer(create_uniform(aspect, state));

    auto library = ctx.create_library("examples/04_shadow_map/shader.crystallib");

    shadow_texture_     = ctx.create_texture(crystal::TextureDesc{
        /* .width  = */ 2048,
        /* .height = */ 2048,
        // /* .format = */ crystal::TextureFormat::RGBA8u,
        /* .format = */ crystal::TextureFormat::Depth32f,
        /* .sample = */ crystal::TextureSample::Linear,
        /* .repeat = */ crystal::TextureRepeat::Clamp,
    });
    shadow_render_pass_ = ctx.create_render_pass(
        {}, std::make_tuple(std::ref(shadow_texture_), crystal::AttachmentDesc{
                                                           .clear       = true,
                                                           .clear_value = {.depth = 1.0f},
                                                       }));
    // shadow_render_pass_ = ctx.create_render_pass({
    //     std::make_tuple(std::ref(shadow_texture_),
    //                     crystal::AttachmentDesc{
    //                         .clear       = true,
    //                         .clear_value = {.color = {0.5f, 0.5f, 0.5f, 1.0f}},
    //                     }),
    // });
    shadow_pipeline_  = ctx.create_pipeline(library, shadow_render_pass_, shadow_desc);
    combine_pipeline_ = ctx.create_pipeline(library, ctx.screen_render_pass(), combine_desc);

    constexpr float SCALE                = 0.25f;
    auto            cube_vertex_buffer   = ctx.create_vertex_buffer({
        // clang-format off
        // -x
        Vertex{glm::vec4(-SCALE,  SCALE, -SCALE, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE, -SCALE, -SCALE, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE,  SCALE,  SCALE, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE, -SCALE,  SCALE, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f)},
        // +x
        Vertex{glm::vec4( SCALE, -SCALE, -SCALE, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE,  SCALE, -SCALE, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE, -SCALE,  SCALE, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE,  SCALE,  SCALE, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        // -y
        Vertex{glm::vec4(-SCALE, -SCALE,  SCALE, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE, -SCALE, -SCALE, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE, -SCALE,  SCALE, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE, -SCALE, -SCALE, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 1.0f)},
        // +y
        Vertex{glm::vec4(-SCALE,  SCALE, -SCALE, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE,  SCALE,  SCALE, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE,  SCALE, -SCALE, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        Vertex{glm::vec4( SCALE,  SCALE,  SCALE, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        // -z
        Vertex{glm::vec4( SCALE, -SCALE, -SCALE, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE, -SCALE, -SCALE, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)},
        Vertex{glm::vec4( SCALE,  SCALE, -SCALE, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE,  SCALE, -SCALE, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)},
        // +z
        Vertex{glm::vec4(-SCALE, -SCALE,  SCALE, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        Vertex{glm::vec4( SCALE, -SCALE,  SCALE, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        Vertex{glm::vec4(-SCALE,  SCALE,  SCALE, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        Vertex{glm::vec4( SCALE,  SCALE,  SCALE, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        // clang-format on
    });
    auto cube_instance_buffer = ctx.create_vertex_buffer({
        // clang-format off
        Instance{glm::vec4(-0.5f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Instance{glm::vec4( 0.5f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
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
            std::make_tuple(1, std::ref(cube_instance_buffer)),
        },
        cube_index_buffer);

    constexpr float FLOOR                 = 4.0f;
    auto            floor_vertex_buffer   = ctx.create_vertex_buffer({
        // clang-format off
        Vertex{glm::vec4(-FLOOR, -0.75f,  FLOOR, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)},
        Vertex{glm::vec4( FLOOR, -0.75f,  FLOOR, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)},
        Vertex{glm::vec4(-FLOOR, -0.75f, -FLOOR, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)},
        Vertex{glm::vec4( FLOOR, -0.75f, -FLOOR, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)},
        // clang-format on
    });
    auto floor_instance_buffer = ctx.create_vertex_buffer({
        // clang-format off
        Instance{glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)},
        // clang-format on
    });
    floor_mesh_ = ctx.create_mesh({
        std::make_tuple(0, std::ref(floor_vertex_buffer)),
        std::make_tuple(1, std::ref(floor_instance_buffer)),
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

    cmd.use_render_pass(shadow_render_pass_);
    cmd.use_pipeline(shadow_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0);
    cmd.draw(cube_mesh_, 34, 2 /* instances */);
    cmd.draw(floor_mesh_, 4, 1 /* instances */);

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(combine_pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0);
    cmd.use_texture(shadow_texture_, 0);
    cmd.draw(cube_mesh_, 34, 2 /* instances */);
    cmd.draw(floor_mesh_, 4, 1 /* instances */);
  }
};

}  // namespace examples::shadow_map
