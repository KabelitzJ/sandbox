#version 450

layout(location = 0) out vec2 out_uv;

const vec2 POSITIONS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2( 1.0, -1.0),
  vec2( 1.0,  1.0),
  vec2(-1.0, -1.0),
  vec2( 1.0,  1.0),
  vec2(-1.0,  1.0)
);

const vec2 UVS[6] = vec2[](
  vec2(0.0, 0.0),
  vec2(1.0, 0.0),
  vec2(1.0, 1.0),
  vec2(0.0, 0.0),
  vec2(1.0, 1.0),
  vec2(0.0, 1.0)
);

void main() {
  out_uv = UVS[gl_VertexIndex];

  gl_Position = vec4(POSITIONS[gl_VertexIndex], 0.0, 1.0);
}
