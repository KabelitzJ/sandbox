#version 450

#include "../common/quad.glsl"

layout(location = 0) out vec2 out_uv;

void main() {
  out_uv = BASE_QUAD[gl_VertexIndex].uv;

  gl_Position = vec4(BASE_QUAD[gl_VertexIndex].position, 0.0, 1.0);
}
