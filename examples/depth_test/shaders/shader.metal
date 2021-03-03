#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Uniform {
  float4x4 matrix;
};

struct Vertex {
  float4 position [[ attribute(0) ]];
  float4 color    [[ attribute(1) ]];
};

struct InOut {
  float4 position [[ position ]];
  float4 color;
};

vertex InOut depth_test_vert(Vertex in [[ stage_in ]], constant Uniform& u [[ buffer(1) ]]) {
  InOut out;
  out.position = u.matrix * in.position;
  out.color    = in.color;
  return out;
}

fragment float4 depth_test_frag(InOut in [[ stage_in ]], constant Uniform& u [[ buffer(1) ]]) {
  return float4(in.color);
}
