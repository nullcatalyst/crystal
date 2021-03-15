#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Uniform {
  float4x4 cube_matrix;
  float4x4 quad_matrix;
};

////////////////////////////////////////////////////////////////
// Cube

struct CubeVertex {
  float4 position [[ attribute(0) ]];
  float4 color    [[ attribute(1) ]];
};

struct CubeInOut {
  float4 position [[ position ]];
  float4 color;
};

vertex CubeInOut cube_vert(CubeVertex in [[ stage_in ]], constant Uniform& uniform [[ buffer(1) ]]) {
  CubeInOut out;
  out.position = uniform.cube_matrix * in.position;
  out.color    = in.color;
  return out;
}

fragment float4 cube_frag(CubeInOut in [[ stage_in ]], constant Uniform& uniform [[ buffer(1) ]]) {
  return float4(in.color);
}

////////////////////////////////////////////////////////////////
// Quad

struct QuadVertex {
  float4 position  [[ attribute(0) ]];
  float4 tex_coord [[ attribute(1) ]];
};

struct QuadInOut {
  float4 position [[ position ]];
  float4 tex_coord;
};

vertex QuadInOut quad_vert(QuadVertex in [[ stage_in ]], constant Uniform& uniform [[ buffer(1) ]]) {
  QuadInOut out;
  out.position  = uniform.quad_matrix * in.position;
  out.tex_coord = in.tex_coord;
  return out;
}

fragment float4 quad_frag(
    QuadInOut in [[ stage_in ]],
    constant Uniform& uniform [[ buffer(1) ]],
    texture2d<half> image [[ texture(0) ]]) {
  constexpr sampler texture_sampler;
  // return float4(1.0, 1.0, 1.0, 1.0);
  return float4(image.sample(texture_sampler, in.tex_coord.xy));
}
