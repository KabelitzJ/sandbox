#version 450 core

#extension GL_EXT_buffer_reference: enable

struct vertex {
  vec4 position;
  vec4 color;
}; // struct vertex

layout(location = 0) out vec4 out_color;

layout(binding = 0, std430) readonly buffer buffer_vertex_data {
  vertex vertices[];
} vertex_data;

layout(push_constant) uniform push_data {
	mat4 mvp;
} push;

void main() {
  vertex vertex = vertex_data.vertices[gl_VertexIndex];

  out_color = vertex.color;

  gl_Position = push.mvp * vertex.position; 
}
