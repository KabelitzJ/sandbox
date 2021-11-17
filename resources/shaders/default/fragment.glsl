#version 460 core

// uniform fragment_uniforms {
//   vec4 color;
//   sampler2D texture;
// } uniforms;

// in vertex_data {
//   vec2 uv;
// } in_data;

out vec4 out_color;

void main() {
  out_color = vec4(1.0, 1.0, 1.0, 1.0);
}
