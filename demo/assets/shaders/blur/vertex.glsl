#version 450 core

#include "../common/quad.glsl"

layout(location = 0) out vec2 out_uv;

void main() {
  // out_uv = BASE_QUAD[gl_VertexIndex].uv;
  // gl_Position = vec4(BASE_QUAD[gl_VertexIndex].position, 0.0, 1.0);

  out_uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(out_uv * 2.0f - 1.0f, 0.0f, 1.0f);
}
