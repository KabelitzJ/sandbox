#version 450 core

#extension GL_EXT_buffer_reference: enable

struct vertex {
  vec4 position;
  vec4 color;
}; // struct vertex

layout(location = 0) out vec4 out_color;

layout(std430, buffer_reference) readonly buffer vertex_buffer {
  vertex vertices[];
};

layout(push_constant) uniform push_data {
	mat4 mvp;
  vertex_buffer vertex_buffer;
} push;

void main() {
  vertex vertex = push.vertex_buffer.vertices[gl_VertexIndex];

  out_color = vertex.color;

  gl_Position = push.mvp * vertex.position; 
}
