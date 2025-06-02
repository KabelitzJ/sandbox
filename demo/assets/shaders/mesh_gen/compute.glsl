#version 460 core

layout(local_size_x = 32) in;

struct vertex {
  vec3 position;
}; // struct vertex

layout(set = 0, binding = 0) uniform uniform_config {
  vec4 offset;
} config;

layout(binding = 1, std430) buffer buffer_out_vertices {
  vertex vertices[];
} out_vertices;

void main() {
  uint id = gl_GlobalInvocationID.x;

  out_vertices.vertices[id].position = vec3(config.offset.xyz);
}
