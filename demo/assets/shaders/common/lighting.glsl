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
  vec3 lightDir = normalize(light.position - world_position);
  vec3 viewDir = normalize(view_position - world_position);
  vec3 halfwayDir = normalize(lightDir + viewDir);

  // ambient
  float ambientStrength = 0.1;
  vec4 ambient = ambientStrength * light.color;
  
  // diffuse 
  vec3 norm = normalize(normal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = diff * light.color;
  
  // specular
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec4 specular = spec * light.color;  

  return light_result(ambient, diffuse, specular);
}

// Function to calculate lighting from a directional light
light_result calculate_directional_light(directional_light light, vec3 world_position, vec3 normal, vec3 view_position, material material) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(view_position - world_position);
  vec3 halfwayDir = normalize(lightDir + viewDir);

  // ambient
  float ambientStrength = 0.1;
  vec4 ambient = ambientStrength * light.color;

  // diffuse
  vec3 norm = normalize(normal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = diff * light.color;

  // specular
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec4 specular = spec * light.color;

  return light_result(ambient, diffuse, specular);
}

#endif // COMMON_LIGHTING_GLSL
