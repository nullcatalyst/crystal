#version 420 core

layout(binding = 0) uniform sampler2D t_image;

layout(location = 0) in vec4 v_tex_coord;

layout(location = 0) out vec4 o_color;

void main() {
  o_color = texture(t_image, v_tex_coord.xy);
}
