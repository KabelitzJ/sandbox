#version 460 core

#include <libsbx/common/lighting.glsl>
#include <libsbx/common/material.glsl>
#include <libsbx/common/shadow.glsl>
#include <libsbx/common/depth.glsl>
#include <libsbx/common/constants.glsl>

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_color;

// === Uniforms ===
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

// === G-buffer ===
layout(set = 0, binding = 2) uniform sampler2D albedo_image;
layout(set = 0, binding = 3) uniform sampler2D position_image;
layout(set = 0, binding = 4) uniform sampler2D normal_image;
layout(set = 0, binding = 5) uniform sampler2D material_image;
layout(set = 0, binding = 6) uniform usampler2D object_id_image;
layout(set = 0, binding = 7) uniform sampler2D shadow_image;

// === Constants ===
const float MIN_ROUGHNESS = 0.15;
const float MAX_SHININESS = 256.0;
const float MIN_SHININESS = 2.0;
const vec3 DEFAULT_F0 = vec3(0.04);

const mat4 BIAS_MATRIX = mat4(
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
);

// === Lighting Function ===
vec3 apply_blinn_phong_lighting(vec3 normal, vec3 view_direction, vec3 light_direction, vec3 light_color, vec3 albedo, float metallic, float roughness) {
  vec3 half_vector = normalize(light_direction + view_direction);

  float ndl = max(dot(normal, light_direction), 0.0);
  float ndh = max(dot(normal, half_vector), 0.0);

  float shininess = mix(MAX_SHININESS, MIN_SHININESS, roughness);
  float specular_intensity = pow(ndh, shininess) * (1.0 - roughness);

  vec3 f0 = mix(DEFAULT_F0, albedo, metallic);
  vec3 specular = f0 * specular_intensity;

  float diffuse_strength = 1.0 - max(max(f0.r, f0.g), f0.b);
  vec3 diffuse = albedo * diffuse_strength * ndl;

  return (diffuse + specular) * light_color;
}

float shadow(vec3 world_position, vec3 normal) {
  vec4 shadow_coord = BIAS_MATRIX * scene.light_space * vec4(world_position, 1.0);
  vec4 normalized_shadow_coord = shadow_coord / shadow_coord.w;

  float closest_depth = texture(shadow_image, normalized_shadow_coord.xy).r;
  float current_depth = normalized_shadow_coord.z;

  float bias = max(0.0005 * (1.0 - dot(normal, normalize(scene.light_direction))), 0.0001);
  float shadow = 0.0;
  vec2 texel_size = 1.0 / textureSize(shadow_image, 0);

  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      vec2 offset = vec2(x, y) * texel_size;
      float pcf_depth = texture(shadow_image, shadow_coord.xy + offset).r;

      shadow += current_depth - bias > pcf_depth ? 0.0 : 1.0;
    }
  }

  return (shadow / 9.0);
}

// === Main ===
void main() {
  // --- G-buffer sampling ---
  vec4 albedo_sample = texture(albedo_image, in_uv);
  vec3 albedo = albedo_sample.rgb;
  vec3 world_position = texture(position_image, in_uv).xyz;
  vec3 normal = normalize(texture(normal_image, in_uv).xyz);
  vec3 mrao = texture(material_image, in_uv).rgb;

  float metallic = clamp(mrao.r, 0.0, 1.0);
  float roughness = clamp(mrao.g, MIN_ROUGHNESS, 1.0);
  float ao = clamp(mrao.b, 0.0, 1.0);

  vec3 view_direction = normalize(scene.camera_position - world_position);

  // --- Shadows (PCF) ---
  float shadow = shadow(world_position, normal);

  // --- Ambient light ---
  vec3 ambient = albedo * 0.04 * ao;

  // --- Directional light ---
  vec3 directional_light_direction = normalize(-scene.light_direction);
  vec3 directional = apply_blinn_phong_lighting(normal, view_direction, directional_light_direction, scene.light_color.rgb, albedo, metallic, roughness) * shadow;

  // --- Point lights ---
  vec3 point_lighting = vec3(0.0);

  for (uint i = 0u; i < scene.point_light_count; ++i) {
    point_light light = point_lights.data[i];

    vec3 to_light = light.position - world_position;
    float distance = max(length(to_light), 0.001);
    vec3 light_direction = normalize(to_light);

    float attenuation = clamp(1.0 - distance / light.radius, 0.0, 1.0);
    attenuation *= attenuation;

    vec3 light_color = light.color.rgb * light.color.a * attenuation;
    point_lighting += apply_blinn_phong_lighting(normal, view_direction, light_direction, light_color, albedo, metallic, roughness);
  }

  // --- Final color composition ---
  vec3 final_color = ambient + directional + point_lighting;

  out_color = vec4(final_color, albedo_sample.a);
}
