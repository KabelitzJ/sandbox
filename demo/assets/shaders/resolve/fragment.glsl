#version 460 core

#include <libsbx/common/lighting.glsl>
#include <libsbx/common/material.glsl>
#include <libsbx/common/shadow.glsl>
#include <libsbx/common/depth.glsl>
#include <libsbx/common/constants.glsl>

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(set = 0, binding = 1, std430) readonly buffer buffer_point_lights {
  point_light data[];
} point_lights;

layout(set = 0, binding = 2, input_attachment_index = 0) uniform subpassInput albedo_image;
layout(set = 0, binding = 3, input_attachment_index = 1) uniform subpassInput position_image; 
layout(set = 0, binding = 4, input_attachment_index = 2) uniform subpassInput normal_image;
layout(set = 0, binding = 5, input_attachment_index = 3) uniform subpassInput material_image;

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
  vec2 material = subpassLoad(material_image).xy;

  float metallic = material.x;
  float roughness = material.y;

  vec3 N = normalize(normal);
  vec3 L = normalize(-scene.light_direction);
  vec3 V = normalize(scene.camera_position - world_position);
  vec3 H = normalize(L + V);
  
  float diffuse_strength = max(dot(N, L), 0.0);
  float specular_strength = pow(max(dot(N, H), 0.0), 32.0);

  vec4 ambient = AMBIENT_COLOR * albedo;
  vec4 diffuse = diffuse_strength * albedo * scene.light_color;
  vec4 specular = SPECULAR_COLOR * specular_strength * albedo;

  vec4 color = ambient + diffuse + specular;

  out_color = color;
}
