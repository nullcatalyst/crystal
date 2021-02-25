#pragma once

#include <cstdint>
#include <initializer_list>

namespace crystal {

enum class CullMode : uint32_t {
  None  = 0,
  Front = 1,
  Back  = 2,
};

enum class Winding : uint32_t {
  CounterClockwise = 0,
  Clockwise        = 1,
};

enum class DepthTest : uint32_t {
  Never        = 0,
  Less         = 1,
  Equal        = 2,
  LessEqual    = 3,
  Greater      = 4,
  NotEqual     = 5,
  GreaterEqual = 6,
  Always       = 7,
};

enum class DepthWrite : uint32_t {
  Disable = 0,
  Enable  = 1,
};

enum class AlphaBlend : uint32_t {
  Zero             = 0,
  One              = 1,
  SrcColor         = 2,
  OneMinusSrcColor = 3,
  DstColor         = 4,
  OneMinusDstColor = 5,
  SrcAlpha         = 6,
  OneMinusSrcAlpha = 7,
  DstAlpha         = 8,
  OneMinusDstAlpha = 9,
};

struct UniformBinding {
  uint32_t id;
};

struct TextureBinding {
  uint32_t id;
};

struct VertexAttributeDesc {
  uint32_t id;
  // VertexFormat format;
  uint32_t offset;
  uint32_t buffer_index;
};

enum class StepFunction : uint32_t {
  PerVertex   = 0,
  PerInstance = 1,
};

struct VertexBufferDesc {
  uint32_t     buffer_index;
  uint32_t     stride;
  StepFunction step_function;
};

struct PipelineDesc {
  const char* const                          vertex;
  const char* const                          fragment;
  CullMode                                   cull_mode;
  Winding                                    winding;
  DepthTest                                  depth_test;
  DepthWrite                                 depth_write;
  AlphaBlend                                 blend_src;
  AlphaBlend                                 blend_dst;
  std::initializer_list<UniformBinding>      uniform_bindings;
  std::initializer_list<TextureBinding>      texture_bindings;
  std::initializer_list<VertexAttributeDesc> vertex_attributes;
  std::initializer_list<VertexBufferDesc>    vertex_buffers;
};

}  // namespace crystal
