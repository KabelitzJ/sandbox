#version 450 core

#extension GL_EXT_buffer_reference: enable

struct vertex {
  vec4 position;
  vec4 color;
}; // struct vertex

layout(location = 0) out vec4 out_color;

layout(buffer_reference, buffer_reference_align = 8, std430) readonly buffer buffer_reference_vertex_data {
  vertex vertices[];
};

layout(push_constant) uniform push_data {
	mat4 mvp;
  buffer_reference_vertex_data vertex_data;
} push;

void main() {
  vertex vertex = push.vertex_data.vertices[gl_VertexIndex];

  out_color = vertex.color;

  gl_Position = push.mvp * vertex.position; 
}
