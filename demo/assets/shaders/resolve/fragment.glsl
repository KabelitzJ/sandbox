#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/shadow.glsl"
#include "../common/depth.glsl"
#include "../common/constants.glsl"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(binding = 1, std430) readonly buffer buffer_point_lights {
  point_light data[];
} point_lights;

layout(binding = 2, input_attachment_index = 0) uniform subpassInput position_image; 
layout(binding = 3, input_attachment_index = 1) uniform subpassInput normal_image;
layout(binding = 4, input_attachment_index = 2) uniform subpassInput albedo_image;

layout(binding = 5) uniform sampler2D shadow_map_image;

const material DEFAULT_MATERIAL = material(
  vec4(1.0, 1.0, 1.0, 1.0),     // Ambient color
  vec4(1.0, 1.0, 1.0, 1.0),     // Specular color
  0.4,                          // Metallic
  0.7,                          // Roughness
  0.7                           // Ambient occlusion
);

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

const vec4 AMBIENT_COLOR = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 SPECULAR_COLOR = vec4(0.9, 0.9, 0.9, 1.0);
const float GLOSSINESS = 32.0;
const vec4 RIM_COLOR = vec4(1.0, 1.0, 1.0, 1.0);
const float RIM_STRENGTH = 0.716;
const float RIM_THRESHOLD = 0.1;

void main() {
  vec3 world_position = subpassLoad(position_image).xyz;
  vec3 normal = normalize(subpassLoad(normal_image).xyz);
  vec4 albedo = subpassLoad(albedo_image);

  vec4 light_space_position = DEPTH_BIAS * scene.light_space * vec4(world_position, 1.0);

  float shadow = calculate_shadow_pcf(shadow_map_image, light_space_position, normal, scene.light_direction);

  vec3 light_direction = normalize(-scene.light_direction);
  vec3 view_direction = normalize(scene.camera_position - world_position);
  vec3 halfway_direction = normalize(light_direction + view_direction);

  float n_dot_l = dot(normal, halfway_direction);
  float light_intensity = smoothstep(0, 0.01, n_dot_l * shadow);

  vec4 light = scene.light_color * light_intensity;

  float specular_factor = pow(n_dot_l * light_intensity, GLOSSINESS * GLOSSINESS);
  float specular_intensity = smoothstep(0.005, 0.01, specular_factor);
  vec4 specular = SPECULAR_COLOR * specular_intensity;

  float rim_factor = (1.0 - dot(view_direction, normal)) * pow(n_dot_l, RIM_THRESHOLD);
  float rim_intensity = smoothstep(RIM_STRENGTH - 0.01, RIM_STRENGTH + 0.01, rim_factor);
  vec4 rim = RIM_COLOR * rim_intensity;

  // out_color = vec4(normal, 1.0f);
  out_color = albedo * (AMBIENT_COLOR + light + specular + rim);

  // vec4 total_light = vec4(0.0);

  // directional_light light = directional_light(scene.light_direction, scene.light_color);

  // light_result result = calculate_directional_light(light, world_position, normal, scene.camera_position, DEFAULT_MATERIAL);

  // total_light += (result.ambient + (result.diffuse + result.specular)) * albedo;

  // for (uint i = 0; i < min(scene.point_light_count, MAX_POINT_LIGHTS); i++) {
  //   point_light light = point_lights.data[i];
  //
  //   light_result result = calculate_point_light(light, world_position, normal, scene.camera_position, DEFAULT_MATERIAL);
  //
  //   total_light += (result.ambient + (result.diffuse + result.specular)) * albedo;
  // }

  // out_color = total_light;
}
