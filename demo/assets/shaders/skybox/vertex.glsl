#version 450 core

#extension GL_EXT_buffer_reference : enable

struct vertex {
	float position_x;
	float position_y;
	float position_z;
}; // struct vertex

vec3 position_from_vertex(vertex vertex) {
  return vec3(vertex.position_x, vertex.position_y, vertex.position_z);
}

layout(buffer_reference, std430) readonly buffer vertex_buffer { 
	vertex vertices[];
}; // buffer vertex_buffer

layout(set = 0, binding = 0) uniform uniform_scene {
  mat4 model;
  mat4 view;
  mat4 projection;
  vec4 tint;
} scene;

layout(push_constant) uniform constants {	
  vertex_buffer vertex_buffer;
} push_constants;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;

void main() {
  mat4 view_no_translation = mat4(mat3(scene.view));

  vertex vertex = push_constants.vertex_buffer.vertices[gl_VertexIndex];

  vec3 in_position = position_from_vertex(vertex);

  vec4 position = scene.projection * view_no_translation * vec4(in_position, 1.0);

  out_position = in_position;
  out_color = scene.tint;

  gl_Position = position.xyww;
}
