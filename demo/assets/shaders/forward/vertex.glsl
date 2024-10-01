#version 450

struct per_mesh_data {
  mat4 model;
  mat4 normal;
  vec4 tint;
  vec4 material; // x: metallic, y: roughness, zw: padding
  vec4 image_indices;
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_color;
layout(location = 4) out vec4 out_material;
layout(location = 5) out flat uint out_albedo_image_index;
layout(location = 6) out flat uint out_normal_image_index;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(binding = 1, std430) readonly buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  out_position = world_position;
  out_normal = normalize(vec3(data.normal * vec4(in_normal, 1.0)));
  out_uv = in_uv;

  out_color = data.tint;
  out_material = data.material;

  out_albedo_image_index = uint(data.image_indices.x);
  out_normal_image_index = uint(data.image_indices.y);

  gl_Position = scene.projection * scene.view * vec4(world_position, 1.0);
}
