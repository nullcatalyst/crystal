#pragma once

#include "crystal/crystal.hpp"
#include "examples/03_depth_test/shader.hpp"
#include "examples/03_depth_test/state.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace examples::depth_test {

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
  typename Ctx::Pipeline      pipeline_;
  typename Ctx::Mesh          mesh_;

public:
  ViewImpl(Ctx& ctx, const State& state) : ctx_(ctx) {
    const auto aspect = static_cast<float>(ctx_.screen_width()) / ctx_.screen_height();
    uniform_buffer_   = ctx.create_uniform_buffer(create_uniform(aspect, state));

    auto library = ctx.create_library("examples/03_depth_test/shader.crystallib");
    pipeline_    = ctx.create_pipeline(library, ctx.screen_render_pass(), cube_desc);

    constexpr float SCALE           = 0.25f;
    auto            vertex_buffer   = ctx.create_vertex_buffer({
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
    auto instance_buffer = ctx.create_vertex_buffer({
        // clang-format off
        Instance{glm::vec4(-0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        Instance{glm::vec4( 0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
        // clang-format on
    });
    auto index_buffer = ctx.create_index_buffer({
        // clang-format off
             0,  1,  2,  3,  3,
         4,  4,  5,  6,  7,  7,
         8,  8,  9, 10, 11, 11,
        12, 12, 13, 14, 15, 15,
        16, 16, 17, 18, 19, 19,
        20, 20, 21, 22, 23,
        // clang-format on
    });
    mesh_ = ctx.create_mesh(
        {
            std::make_tuple(0, std::ref(vertex_buffer)),
            std::make_tuple(1, std::ref(instance_buffer)),
        },
        index_buffer);
  }

  ~ViewImpl() { ctx_.wait(); }

  virtual void frame(const State& state) override {
    auto cmd = ctx_.next_frame();

    ctx_.set_clear_color(ctx_.screen_render_pass(), 0,
                         crystal::ClearValue{.color = {0.25f, 0.25f, 0.25f, 0.0f}});

    {  // Update uniform buffer.
      const auto aspect =
          static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
      ctx_.update_uniform_buffer(uniform_buffer_, create_uniform(aspect, state));
    }

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0);
    cmd.draw(mesh_, 34, 2 /* instances */);
  }

private:
  static Uniform create_uniform(float aspect, State state) {
    return Uniform{
        /* .projection_matrix = */ glm::perspectiveRH_ZO(glm::radians(60.0f), aspect, 0.01f,
                                                         10.0f) *
            glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -2.0f)),
        /* .rotation_matrix   = */
        glm::rotate(
            glm::rotate(glm::identity<glm::mat4>(), state.pitch_angle, glm::vec3(1.0f, 0.0f, 0.0f)),
            state.yaw_angle, glm::vec3(0.0f, 1.0f, 0.0f)),
        /* .orbit_matrix      = */
        glm::rotate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(15.0f),
                                glm::vec3(1.0f, 0.0f, 0.0f)),
                    state.orbit_angle, glm::vec3(0.0f, 1.0f, 0.0f)),
    };
  }
};

}  // namespace examples::depth_test
