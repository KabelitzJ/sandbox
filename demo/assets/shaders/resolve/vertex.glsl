#version 460 core

layout(location = 0) out vec2 out_uv;

vec2 positions[3] = vec2[](
  vec2(-1.0, -1.0),
  vec2( 3.0, -1.0),
  vec2(-1.0,  3.0)
);

void main() {
  vec2 position = positions[gl_VertexIndex];

  out_uv = (position + 1.0) * 0.5;

  gl_Position = vec4(position, 0.0, 1.0);
}
