#version 450

struct per_mesh_data {
  mat4 model;
  vec4 material; // rgb = unused, a = flexibility
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform uniform_scene {
  mat4 light_space;
  float time;
} scene;

layout(binding = 1) buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

const float PI = 3.1415926535897932384626433832795;
const float MAX_ANCHOR_HEIGHT = 2.0;
const float ANCHOR_HEIGHT = 0.0;

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

  gl_Position = scene.light_space * vec4(wind_effect(world_position, data.material.a), 1.0);
}
