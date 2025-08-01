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

layout(set = 0, binding = 2) uniform sampler2D albedo_image;
layout(set = 0, binding = 3) uniform sampler2D position_image;
layout(set = 0, binding = 4) uniform sampler2D normal_image;
layout(set = 0, binding = 5) uniform sampler2D material_image;
layout(set = 0, binding = 6) uniform usampler2D object_id_image;

// Constants
const float MIN_ROUGHNESS = 0.15;
const float MAX_SHININESS = 256.0;
const float MIN_SHININESS = 2.0;
const vec3 DEFAULT_F0 = vec3(0.04);

// Utility
uvec2 get_object_id() {
  return texelFetch(object_id_image, ivec2(gl_FragCoord.xy), 0).xy;
}

// Lighting function
vec3 apply_blinn_phong_lighting(vec3 normal, vec3 view_dir, vec3 light_dir, vec3 light_color, vec3 albedo, float metallic, float roughness) {
  vec3 half_vector = normalize(light_dir + view_dir);

  float ndl = max(dot(normal, light_dir), 0.0);
  float ndh = max(dot(normal, half_vector), 0.0);

  float shininess = mix(MAX_SHININESS, MIN_SHININESS, roughness);
  float specular_intensity = pow(ndh, shininess);

  // damp shiny blobs on rough surfaces
  specular_intensity *= 1.0 - roughness;

  vec3 f0 = mix(DEFAULT_F0, albedo, metallic);
  vec3 specular = f0 * specular_intensity;

  // conserve energy
  float diffuse_strength = 1.0 - max(max(f0.r, f0.g), f0.b);
  vec3 diffuse = albedo * diffuse_strength * ndl;

  return (diffuse + specular) * light_color;
}

void main() {
  // === G-buffer sampling ===
  vec4 albedo_sample = texture(albedo_image, in_uv);
  vec3 albedo = albedo_sample.rgb;
  vec3 world_position = texture(position_image, in_uv).xyz;
  vec3 normal = normalize(texture(normal_image, in_uv).xyz);
  vec3 mrao = texture(material_image, in_uv).rgb;

  float metallic = clamp(mrao.r, 0.0, 1.0);
  float roughness = clamp(mrao.g, MIN_ROUGHNESS, 1.0);
  float ambient_occlusion = clamp(mrao.b, 0.0, 1.0);

  vec3 view_direction = normalize(scene.camera_position - world_position);

  // === Ambient lighting ===
  vec3 ambient = albedo * 0.04 * ambient_occlusion;

  // === Directional light ===
  vec3 dir_light_dir = normalize(-scene.light_direction);
  vec3 directional_light = apply_blinn_phong_lighting(normal, view_direction, dir_light_dir, scene.light_color.rgb, albedo, metallic, roughness);

  // === Point lights ===
  vec3 point_light_result = vec3(0.0);
  for (uint i = 0u; i < scene.point_light_count; ++i) {
    point_light light = point_lights.data[i];

    vec3 to_light = light.position - world_position;
    float distance = max(length(to_light), 0.001);
    vec3 light_dir = normalize(to_light);

    float attenuation = clamp(1.0 - distance / light.radius, 0.0, 1.0);
    attenuation *= attenuation;
    float intensity = light.color.a;
    vec3 light_color = light.color.rgb * intensity * attenuation;

    point_light_result += apply_blinn_phong_lighting(normal, view_direction, light_dir, light_color, albedo, metallic, roughness);
  }

  // === Final color ===
  vec3 final_color = ambient + directional_light + point_light_result;

  out_color = vec4(final_color, albedo_sample.a);
}
