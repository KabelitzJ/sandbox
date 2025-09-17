#version 460 core

#include <grid/config.glsl>

layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec2 out_camera_position;

layout(push_constant) uniform push_data {
  mat4 mvp;
  vec4 camera_position;
  vec4 origin;
} push;

const vec3 vertices[4] = vec3[4](
  vec3(-1.0, 0.0, -1.0),
  vec3( 1.0, 0.0, -1.0),
  vec3( 1.0, 0.0, 1.0),
  vec3(-1.0, 0.0, 1.0)
);

const int indices[6] = int[6](0, 1, 2, 2, 3, 0);

void main() {
  int index = indices[gl_VertexIndex];

  vec3 position = vertices[index] * grid_size;

  position.x += push.camera_position.x;
  position.z += push.camera_position.z;
  position += push.origin.xyz;

  out_camera_position = push.camera_position.xz;
  out_uv = position.xz;

  gl_Position = push.mvp * vec4(position, 1.0);
}
