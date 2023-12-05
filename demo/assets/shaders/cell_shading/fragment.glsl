#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_light_space_position;
layout(location = 4) in vec4 in_tint;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  mat4 light_space;
  vec3 light_direction;
  vec4 light_color;
} scene;

layout(binding = 2) uniform sampler2D image;
layout(binding = 3) uniform sampler2D shadow_map;

const material default_material = material(
  vec4(1.0, 1.0, 1.0, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  vec4(0.5, 0.5, 0.5, 1.0),
  16.0
);

vec4 phong_lighting(vec3 light_direction, vec3 view_direction, vec3 normal, material material) {
  // Ambient
  vec4 ambient_color = scene.light_color * material.ambient * 0.2;

  // Diffuse
  float diffuse_factor = max(dot(light_direction, normal), 0.0) * 0.8;
  vec4 diffuse_color = diffuse_factor * scene.light_color * material.diffuse;

  // Specular
  vec3 halfway_direction = normalize(light_direction + view_direction);  
  float specular_factor = pow(max(dot(normal, halfway_direction), 0.0), material.shininess);
  vec4 specular_color = specular_factor * scene.light_color * material.specular; 

  return ambient_color + diffuse_color + specular_color;
}

float pcf_shadow(vec3 light_direction) {
  vec2 texture_size = textureSize(shadow_map, 0);
  vec2 texel_size = 1.0 / texture_size;

  vec3 coordinates = in_light_space_position.xyz / in_light_space_position.w;

  if (coordinates.z > 1.0 || coordinates.z < -1.0) {
    return 0.0;
  }

  float shadow = 0.0;

  float bias = max(0.005 * (1.0 - dot(in_normal, light_direction)), 0.0005);
  // float bias = 0.001;
  
  float current_depth = coordinates.z;
  
  int count = 0;
  int range = 3;

  for (int x = -range; x <= range; ++x) {
    for (int y = -range; y <= range; ++y) {
      float pcf_depth = texture(shadow_map, coordinates.xy + vec2(x, y) * texel_size).r;
      shadow += (current_depth - bias) > pcf_depth ? 1.0 : 0.0;
      ++count;
    }
  }

  return shadow / float(count);
}

void main() {
  vec3 light_direction = normalize(-scene.light_direction);
  vec3 view_direction = normalize(scene.camera_position - in_position);

  // Calculate lighting
  vec4 lighting = phong_lighting(light_direction, view_direction, in_normal, default_material);

  // Calculate shadow
  float shadow_factor = pcf_shadow(light_direction);

  // Sample texture
  vec4 sampled_color = texture(image, in_uv);

  // Apply lighting
  vec4 color = sampled_color * lighting * in_tint;

  out_color = mix(color * (1.0 - shadow_factor), color * 0.2, shadow_factor);

  // out_color = (ambient_color + diffuse_color + specular_color) * sampled_color * (1.0 - shadow_factor);
  // out_color = color * shadow;

  // out_color = vec4(shadow_factor, shadow_factor, shadow_factor, 1.0);
  // out_color = vec4(texture(shadow_map, in_uv).rrr, 1.0);
}
