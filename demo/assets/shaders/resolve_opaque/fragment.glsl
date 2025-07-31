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

const vec4 AMBIENT_COLOR = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 SPECULAR_COLOR = vec4(0.9, 0.9, 0.9, 1.0);
const float GLOSSINESS = 32.0;

float uint_to_float(uint value) {
  return float(value) / 4294967295.0;
}

uvec2 get_object_id() {
  return texelFetch(object_id_image, ivec2(gl_FragCoord.xy), 0).xy;
}

void main() {
  vec4 albedo = texture(albedo_image, in_uv);
  vec3 world_position = texture(position_image, in_uv).xyz;
  vec3 normal = normalize(texture(normal_image, in_uv).xyz);
  vec3 material = texture(material_image, in_uv).xyz;

  float metallic = material.x;
  float roughness = material.y;
  float ambient_occlusion = material.z;

  vec3 view_direction = normalize(scene.camera_position - world_position);

  // Directional light
  vec3 light_direction = normalize(-scene.light_direction);
  vec3 half_direction = normalize(light_direction + view_direction);

  // Ambient with AO
  vec4 ambient = AMBIENT_COLOR * albedo * ambient_occlusion;

  // Diffuse: suppressed for metals
  float diffuse_strength = max(dot(normal, light_direction), 0.0);
  vec4 diffuse = diffuse_strength * albedo * scene.light_color * (1.0 - metallic);

  // Specular: affected by roughness and metallic
  float shininess = mix(256.0, 2.0, roughness);
  float specular_strength = pow(max(dot(normal, half_direction), 0.0), shininess);
  vec3 specular_color = mix(SPECULAR_COLOR.rgb, albedo.rgb, metallic);
  vec4 specular = vec4(specular_color * specular_strength, 1.0);

  // Point lights
  vec3 point_diffuse = vec3(0.0);
  vec3 point_specular = vec3(0.0);

  for (uint i = 0u; i < scene.point_light_count; ++i) {
    point_light light = point_lights.data[i];

    vec3 to_light = light.position - world_position;
    float distance = length(to_light);
    vec3 light_dir = normalize(to_light);

    // Attenuation: fade with distance (smoothstep for soft edge)
    float attenuation = clamp(1.0 - distance / light.radius, 0.0, 1.0);
    // quadratic falloff (optional for realism)
    attenuation *= attenuation;

    // Intensity from color.a (if used, otherwise just use 1.0)
    float intensity = light.color.a;

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    point_diffuse += diff * albedo.rgb * light.color.rgb * intensity * attenuation * (1.0 - metallic);

    // Specular
    vec3 half_dir = normalize(light_dir + view_direction);
    float spec = pow(max(dot(normal, half_dir), 0.0), shininess);
    vec3 spec_col = mix(SPECULAR_COLOR.rgb, albedo.rgb, metallic);

    point_specular += spec_col * spec * light.color.rgb * intensity * attenuation;
  }

  // Ambient (with AO)
  vec4 ambient = AMBIENT_COLOR * albedo * ambient_occlusion;

  // Final color
  vec3 total = ambient.rgb + diffuse.rgb + specular.rgb + point_diffuse + point_specular;

  out_color = vec4(total, albedo.a);
}
