#version 420 core

layout(set = 0, binding = 0) uniform Uniform {
  mat4 u_depth_matrix;
  mat4 u_combine_matrix;
};

// Vertex
layout(location = 0) in vec4 i_position;
layout(location = 1) in vec4 i_color;

// Varyings
layout(location = 0) out vec4 v_color;

out gl_PerVertex { vec4 gl_Position; };

void main() {
  gl_Position = u_depth_matrix * i_position;
  v_color     = i_color;
}
