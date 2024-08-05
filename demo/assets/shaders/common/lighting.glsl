#ifndef COMMON_LIGHTING_GLSL
#define COMMON_LIGHTING_GLSL

#include "../common/material.glsl"

struct point_light {
  vec3 position;
  vec4 color;
  float radius;
}; // point_light

const uint MAX_POINT_LIGHTS = 32;

struct directional_light {
  vec3 direction;
  vec4 color;
}; // directional_light

struct light_result {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
}; // struct light_result

// Function to calculate attenuation based on distance and radius
float calculate_attenuation(float distance, float radius) {
  return 1.0 / (1.0 + (distance / radius) * (distance / radius));
}

// Function to calculate lighting from a point light
light_result calculate_point_light(point_light light, vec3 world_position, vec3 normal, vec3 view_position, material material) {
  light_result result;

  vec3 lightDir = normalize(light.position - world_position);
  float distance = length(light.position - world_position);
  float attenuation = calculate_attenuation(distance, light.radius);

  // Ambient component
  result.ambient = material.albedo * light.color * material.ambient_occlusion * attenuation;

  // Diffuse component
  float diff = max(dot(normal, lightDir), 0.0);
  result.diffuse = (1.0 - material.metallic) * material.albedo * diff * light.color * attenuation;

  // Specular component
  vec3 viewDir = normalize(view_position - world_position);
  vec3 halfDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfDir), 0.0), 1.0 / (material.roughness * material.roughness));
  result.specular = material.specular * spec * light.color * attenuation;

  return result;
}

// Function to calculate lighting from a directional light
light_result calculate_directional_light(directional_light light, vec3 world_position, vec3 normal, vec3 view_position, material material) {
  float intensity = max(dot(normal, -light.direction), 0.0);

  // Calculate the ambient color
  vec4 ambient = (light.color * 0.1) * material.albedo;

  // Calculate the diffuse color
  vec4 diffuse = (light.color * 0.5) * (material.albedo * intensity);

  // Calculate the specular color
  vec3 camera_direction = normalize(vec3(view_position) - world_position);
  vec3 halfway_direction = normalize(light.direction + camera_direction);
  float specular_intensity = pow(max(dot(normal, halfway_direction), 0.0), 32);
  vec4 specular = material.specular * specular_intensity;

  // Calculate the final color
  return light_result(ambient,diffuse, specular);
}

#endif // COMMON_LIGHTING_GLSL
