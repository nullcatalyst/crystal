#pragma once

#include <cstdint>

#include "absl/container/inlined_vector.h"
#include "crystal/opengl/gl.hpp"
#include "crystal/opengl/library.hpp"

#define CRYSTAL_IMPL opengl
#define CRYSTAL_IMPL_PROPERTIES                  \
  struct Binding {                               \
    uint32_t     id;                             \
    uint32_t     offset;                         \
    uint32_t     buffer_index;                   \
    uint32_t     stride;                         \
    StepFunction step_function;                  \
  };                                             \
                                                 \
  static uint32_t next_id_;                      \
                                                 \
  Context*                         ctx_;         \
  uint32_t                         id_;          \
  GLuint                           program_;     \
  CullMode                         cull_mode_;   \
  DepthCompare                     depth_test_;  \
  DepthWrite                       depth_write_; \
  AlphaBlend                       blend_src_;   \
  AlphaBlend                       blend_dst_;   \
  absl::InlinedVector<Binding, 16> bindings_;
#define CRYSTAL_IMPL_CTOR Pipeline(Context& ctx, Library& library, const PipelineDesc& desc);
#define CRYSTAL_IMPL_METHODS
#include "crystal/interface/pipeline.inl"
