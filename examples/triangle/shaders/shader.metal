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

struct TriangleInOut {
    float4 position [[ position ]];
    float4 color;
};

vertex TriangleInOut triangle_vert(Vertex in [[ stage_in ]], constant Uniform& uniform [[ buffer(1) ]]) {
    TriangleInOut out;

    out.position = uniform.matrix * in.position;
    out.color    = in.color;

    return out;
}

fragment float4 triangle_frag(TriangleInOut in [[ stage_in ]], constant Uniform& uniform [[ buffer(1) ]]) {
    return float4(in.color);
}
