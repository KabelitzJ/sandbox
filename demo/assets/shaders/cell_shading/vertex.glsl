#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_light_space_position;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  mat4 light_space;
  vec3 light_direction;
  vec4 light_color;
} scene;

layout(push_constant) uniform uniform_object {
  mat4 model;
  mat4 normal;
} object;

const mat4 depth_bias = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);


void main() {
  out_position = vec3(object.model * vec4(in_position, 1.0));
  out_normal = normalize(mat3(object.normal) * in_normal);
  out_uv = in_uv;
  out_light_space_position = (depth_bias * scene.light_space) * vec4(out_position, 1.0);

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
