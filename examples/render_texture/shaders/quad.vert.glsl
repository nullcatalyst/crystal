#version 420 core

layout(set = 0, binding = 0) uniform Uniform {
  mat4 u_depth_matrix;
  mat4 u_combine_matrix;
};

// Vertex
layout(location = 0) in vec4 i_position;
layout(location = 1) in vec4 i_tex_coord;

// Varyings
layout(location = 0) out vec4 v_tex_coord;

out gl_PerVertex { vec4 gl_Position; };

void main() {
  gl_Position = u_combine_matrix * i_position;
  v_tex_coord = i_tex_coord;
}
