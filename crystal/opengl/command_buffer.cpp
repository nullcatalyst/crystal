#include "crystal/opengl/command_buffer.hpp"

#include <algorithm>  // find_if

#include "crystal/opengl/mesh.hpp"
#include "crystal/opengl/pipeline.hpp"
#include "crystal/opengl/render_pass.hpp"
#include "crystal/opengl/texture.hpp"
#include "crystal/opengl/uniform_buffer.hpp"

#ifdef CRYSTAL_USE_SDL2
#include "SDL.h"
#endif  // ^^^ defined(CRYSTAL_USE_SDL2)

namespace crystal::opengl {

namespace {

constexpr inline GLenum convert_(AlphaBlend blend) {
  switch (blend) {
    case crystal::AlphaBlend::Zero:
      return GL_ZERO;
    case crystal::AlphaBlend::One:
      return GL_ONE;
    case crystal::AlphaBlend::SrcColor:
      return GL_SRC_COLOR;
    case crystal::AlphaBlend::OneMinusSrcColor:
      return GL_ONE_MINUS_SRC_COLOR;
    case crystal::AlphaBlend::DstColor:
      return GL_DST_COLOR;
    case crystal::AlphaBlend::OneMinusDstColor:
      return GL_ONE_MINUS_DST_COLOR;
    case crystal::AlphaBlend::SrcAlpha:
      return GL_SRC_ALPHA;
    case crystal::AlphaBlend::OneMinusSrcAlpha:
      return GL_ONE_MINUS_SRC_ALPHA;
    case crystal::AlphaBlend::DstAlpha:
      return GL_DST_ALPHA;
    case crystal::AlphaBlend::OneMinusDstAlpha:
      return GL_ONE_MINUS_DST_ALPHA;
  }

  util::msg::fatal("unknown blend mode");
}

}  // namespace

#ifdef CRYSTAL_USE_SDL2

CommandBuffer::~CommandBuffer() {
  glFlush();
  SDL_GL_SwapWindow(window_);
}

#else  // ^^^ defined(CRYSTAL_USE_SDL2) / !defined(CRYSTAL_USE_SDL2) vvv

CommandBuffer::~CommandBuffer() { glFlush(); }

#endif  // ^^^ !defined(CRYSTAL_USE_SDL2)

void CommandBuffer::use_render_pass(RenderPass& render_pass) {
  render_pass_ = &render_pass;

  GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, render_pass.framebuffer_), "binding framebuffer");
  GL_ASSERT(glViewport(0, 0, render_pass.width_, render_pass.height_), "changing viewport size");

  for (uint32_t i = 0; i < render_pass.attachment_count_; ++i) {
    GL_ASSERT(glClearBufferfv(GL_COLOR, i, render_pass.clear_colors_[i].array),
              "setting clear color");
  }

  if (render_pass.clear_depth_.clear) {
    GL_ASSERT(glDepthMask(GL_TRUE), "enabling depth writing before clearing the depth");
    GL_ASSERT(glClearDepthf(render_pass.clear_depth_.depth), "setting clear depth");
    GL_ASSERT(glClear(GL_DEPTH_BUFFER_BIT), "clearing frame buffer depth");
  }
}

void CommandBuffer::use_pipeline(Pipeline& pipeline) {
  pipeline_ = &pipeline;

  GL_ASSERT(glUseProgram(pipeline.program_), "changing active shader program");

  switch (pipeline_->cull_mode_) {
    case CullMode::None:
      GL_ASSERT(glDisable(GL_CULL_FACE), "disabling cull face");
      break;
    case CullMode::Front:
      GL_ASSERT(glCullFace(GL_FRONT), "setting cull front face");
      GL_ASSERT(glEnable(GL_CULL_FACE), "enabling cull face");
      break;
    case CullMode::Back:
      GL_ASSERT(glCullFace(GL_BACK), "setting cull back face");
      GL_ASSERT(glEnable(GL_CULL_FACE), "enabling cull face");
      break;
    default:
      GL_ASSERT(glDisable(GL_CULL_FACE), "disabling cull face");
      break;
  }

  if (pipeline_->blend_src_ != AlphaBlend::One || pipeline_->blend_dst_ != AlphaBlend::Zero) {
    GL_ASSERT(glBlendFunc(convert_(pipeline_->blend_src_), convert_(pipeline_->blend_dst_)),
              "setting blend function");
    GL_ASSERT(glEnable(GL_BLEND), "enabling blending");
  } else {
    GL_ASSERT(glDisable(GL_BLEND), "disabling blending");
  }

  GL_ASSERT(glDepthMask(pipeline_->depth_write_ == DepthWrite::Enable),
            pipeline_->depth_write_ == DepthWrite::Enable ? "enabling depth writing"
                                                          : "disabling depth writing");

  if (pipeline_->depth_test_ != DepthTest::Always) {
    GL_ASSERT(glDepthFunc(GL_NEVER + static_cast<uint32_t>(pipeline_->depth_test_)),
              "setting depth test function");
    GL_ASSERT(glEnable(GL_DEPTH_TEST), "enabling depth testing");
  } else {
    GL_ASSERT(glDisable(GL_DEPTH_TEST), "disabling depth testing");
  }
}

void CommandBuffer::use_uniform_buffer(UniformBuffer& uniform_buffer, uint32_t location,
                                       uint32_t binding) {
  GL_ASSERT(glBindBufferBase(GL_UNIFORM_BUFFER, location, uniform_buffer.buffer_),
            "setting uniform buffer base");
  GL_ASSERT(glUniformBlockBinding(pipeline_->program_, location, binding),
            "binding uniform buffer block");
}

void CommandBuffer::use_texture(Texture& texture, uint32_t location, uint32_t binding) {
  // GL_ASSERT(glUniform1i(location, binding), "setting texture uniform");
  GL_ASSERT(glActiveTexture(GL_TEXTURE0 + binding), "setting active texture");
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, texture.texture_), "binding texture");
}

void CommandBuffer::draw(Mesh& mesh, uint32_t vertex_or_index_count, uint32_t instance_count) {
  const auto it = std::find_if(
      mesh.vaos_.begin(), mesh.vaos_.end(),
      [pipeline_id = pipeline_->id_](auto vao) { return vao.pipeline_id == pipeline_id; });
  if (it != mesh.vaos_.end()) {
    GL_ASSERT(glBindVertexArray(it->vao), "binding vertex array");
  } else {
    GLuint vao = 0;
    GL_ASSERT(glGenVertexArrays(1, &vao), "generating vertex array");
    GL_ASSERT(glBindVertexArray(vao), "binding vertex array");

    for (const auto binding : pipeline_->bindings_) {
      const auto it = std::find_if(mesh.bindings_.begin(), mesh.bindings_.end(),
                                   [=, buffer_index = binding.buffer_index](auto binding) {
                                     return binding.buffer_index == buffer_index;
                                   });
      if (it == mesh.bindings_.end()) {
        continue;
      }

      GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, it->vertex_buffer),
                "binding array buffer to vertex array");
      GL_ASSERT(glEnableVertexAttribArray(binding.id), "enabling vertex attribute array");
      GL_ASSERT(
          glVertexAttribPointer(binding.id, 4, GL_FLOAT, GL_FALSE, binding.stride,
                                reinterpret_cast<void*>(static_cast<uintptr_t>(binding.offset))),
          "setting vertex attribute pointer");
      GL_ASSERT(glVertexAttribDivisor(binding.id,
                                      binding.step_function == StepFunction::PerInstance ? 1 : 0),
                "setting vertex attribute divisor");
    }

    if (mesh.index_buffer_ != 0) {
      GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer_), "setting index buffer");
    }

    mesh.bindings_.emplace_back(Mesh::Binding{
        pipeline_->id_,
        vao,
    });
  }

  if (mesh.index_buffer_ != 0) {
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer_), "setting index buffer");
    GL_ASSERT(glDrawElementsInstanced(GL_TRIANGLE_STRIP, vertex_or_index_count, GL_UNSIGNED_SHORT,
                                      nullptr, instance_count),
              "drawing instanced elements");
  } else {
    GL_ASSERT(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertex_or_index_count, instance_count),
              "drawing instanced arrays");
  }
}

}  // namespace crystal::opengl
