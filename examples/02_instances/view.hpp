#pragma once

#include <vector>

#include "crystal/crystal.hpp"
#include "examples/02_instances/shader.hpp"
#include "examples/02_instances/state.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace examples::instances {

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
  typename Ctx::VertexBuffer  instance_buffer_;
  typename Ctx::Mesh          mesh_;

public:
  ViewImpl(Ctx& ctx, const State& state) : ctx_(ctx) {
    const auto aspect = static_cast<float>(ctx.screen_width()) / ctx.screen_height();
    uniform_buffer_   = ctx.create_uniform_buffer(create_uniform(aspect));

    auto library = ctx.create_library("examples/02_instances/shader.crystallib");
    pipeline_    = ctx.create_pipeline(library, ctx.screen_render_pass(), instances_desc);

    auto vertex_buffer = ctx.create_vertex_buffer({
        // clang-format off
        Vertex{glm::vec4{0.75f * -0.866f, 0.75f * -0.5f, 0.0f, 1.0f}},
        Vertex{glm::vec4{0.75f *  0.866f, 0.75f * -0.5f, 0.0f, 1.0f}},
        Vertex{glm::vec4{0.75f *  0.000f, 0.75f *  1.0f, 0.0f, 1.0f}},
        // clang-format on
    });
    instance_buffer_ = ctx.create_vertex_buffer(create_instances(state));
    mesh_              = ctx.create_mesh({
        std::make_tuple(0, std::ref(vertex_buffer)),
        std::make_tuple(1, std::ref(instance_buffer_)),
    });

    ctx_.set_clear_color(ctx_.screen_render_pass(), 0,
                         crystal::ClearValue{.color = {0.25f, 0.25f, 0.25f, 0.0f}});
  }

  ~ViewImpl() { ctx_.wait(); }

  virtual void frame(const State& state) override {
    auto cmd = ctx_.next_frame();

    {  // Update uniform buffer.
      const auto aspect =
          static_cast<float>(ctx_.screen_width()) / static_cast<float>(ctx_.screen_height());
      ctx_.update_uniform_buffer(uniform_buffer_, create_uniform(aspect));
    }

    {  // Update instance buffer.
      ctx_.update_vertex_buffer(instance_buffer_, create_instances(state));
    }

    cmd.use_render_pass(ctx_.screen_render_pass());
    cmd.use_pipeline(pipeline_);
    cmd.use_uniform_buffer(uniform_buffer_, 0);
    cmd.draw(mesh_, 3 /* vertices */, 3 /* instances */);
  }

private:
  static Uniform create_uniform(float aspect) {
    return Uniform{
        /* .matrix = */ glm::scale(glm::ortho(-aspect, aspect, -1.0f, 1.0f),
                                   glm::vec3(1.0f / 2.0f)),
    };
  }

  static std::vector<Instance> create_instances(const State& state) {
    return std::vector<Instance>{
        // Red
        Instance{
            /* .color  = */ glm::vec4{1.0f, 0.0f, 0.0f, 1.0f},
            /* .matrix = */
            glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3{-2.0f, 0.0f, 0.0f}),
                        state.red_angle, glm::vec3{0.0f, 0.0f, 1.0f}),
        },
        // Green
        Instance{
            /* .color  = */ glm::vec4{0.0f, 1.0f, 0.0f, 1.0f},
            /* .matrix = */
            glm::rotate(glm::identity<glm::mat4>(), state.green_angle, glm::vec3{0.0f, 0.0f, 1.0f}),
        },
        // Blue
        Instance{
            /* .color  = */ glm::vec4{0.0f, 0.0f, 1.0f, 1.0f},
            /* .matrix = */
            glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3{2.0f, 0.0f, 0.0f}),
                        state.blue_angle, glm::vec3{0.0f, 0.0f, 1.0f}),
        },
    };
  }
};

}  // namespace examples::instances
