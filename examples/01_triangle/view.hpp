#pragma once

#include "crystal/crystal.hpp"
#include "examples/01_triangle/shader.hpp"
#include "examples/01_triangle/state.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace examples::triangle {

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
    const auto aspect = static_cast<float>(ctx.screen_width()) / ctx.screen_height();
    uniform_buffer_   = ctx.create_uniform_buffer(create_uniform(aspect, state));

    auto library = ctx.create_library("examples/01_triangle/shader.crystallib");
    pipeline_    = ctx.create_pipeline(library, ctx.screen_render_pass(), triangle_desc);

    auto vertex_buffer = ctx.create_vertex_buffer({
        // clang-format off
        Vertex{glm::vec4{0.75f * -0.866f, 0.75f * -0.5f, 0.0f, 1.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{glm::vec4{0.75f *  0.866f, 0.75f * -0.5f, 0.0f, 1.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{glm::vec4{0.75f *  0.000f, 0.75f *  1.0f, 0.0f, 1.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}},
        // clang-format on
    });
    mesh_ = ctx.create_mesh({
        std::make_tuple(0, std::ref(vertex_buffer)),
    });
  }

  ~ViewImpl() { ctx_.wait(); }

  virtual void frame(const State& state) override {
    auto cmd = ctx_.next_frame();

    ctx_.set_clear_color(
        ctx_.screen_render_pass(), 0,
        crystal::ClearValue{
            .color = {0.0f, 0.5f * std::sin(state.angle) * std::sin(state.angle), 0.0f, 0.0f}});

    {  // Update uniform buffer.
      const auto aspect =
          static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
      ctx_.update_uniform_buffer(uniform_buffer_, create_uniform(aspect, state));
    }

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0);
    cmd.draw(mesh_, 3, 1);
  }

private:
  static Uniform create_uniform(float aspect, State state) {
    return Uniform{
        glm::rotate(glm::ortho(-aspect, aspect, -1.0f, 1.0f), state.angle,
                    glm::vec3(0.0f, 0.0f, 1.0f)),
    };
  }
};

}  // namespace examples::triangle
