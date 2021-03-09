#version 420 core

struct Output {
    vec4 color;
};
struct Varyings {
    vec4 position;
    vec4 color;
};
struct Vertex {
    vec4 position;
    vec4 color;
};
struct Uniform {
    mat4 matrix;
};
layout(set=0, binding=0) uniform U0 {
    mat4 matrix;
} _u;

layout(location=0) in vec4 i0_position;
layout(location=1) in vec4 i1_color;

layout(location=0) out vec4 o_color;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    Vertex _in;
    _in.position = i0_position;
    _in.color = i1_color;

    Varyings _out;
    _out.position = _u.matrix*_in.position;
    _out.color = _in.color;
    const Varyings _ = _out;
    gl_Position = _.position;
    o_color = _.color;
    return;
}