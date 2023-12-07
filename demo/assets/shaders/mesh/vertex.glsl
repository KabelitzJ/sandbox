#version 450

struct per_mesh_data {
  mat4 model;
  mat4 normal;
  vec4 material; // rgb = color, a = flexibility
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_light_space_position;
layout(location = 4) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  mat4 light_space;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  float time;
} scene;

layout(binding = 1) buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

const float PI = 3.1415926535897932384626433832795;
const float MAX_ANCHOR_HEIGHT = 2.0;
const float FLEXIBILITY = 0.15;
const float ANCHOR_HEIGHT = 0.0;

const float BRIGHTNESS_EFFECT = 0.5;

vec3 wind_effect(vec3 world_position, float flexibility){
	float height_from_anchor = max(0.0, in_position.y - (ANCHOR_HEIGHT * MAX_ANCHOR_HEIGHT));

	float amplitude = height_from_anchor * flexibility;

	float wave = sin(2.0 * PI * scene.time + (world_position.z + world_position.x) * 0.8);
	float wave2 = sin(2.0 * PI * (scene.time + world_position.z + world_position.x) * 2.0);

	world_position.x += (wave + wave2 * 0.4) * 0.06 * amplitude;
	world_position.z -= (wave - wave2 * 0.4) * 0.03 * amplitude;

	return world_position;
}

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  out_position = wind_effect(world_position, data.material.a);
  out_normal = normalize(mat3(data.normal) * in_normal);
  out_uv = in_uv;
  out_light_space_position = (DEPTH_BIAS * scene.light_space) * vec4(out_position, 1.0);

  float brightness = (data.material.r + data.material.g + data.material.b) / 3.0;
  brightness = (1.0 - BRIGHTNESS_EFFECT) + BRIGHTNESS_EFFECT * brightness;
  out_color = vec4(data.material.rgb * brightness, 1.0);

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
