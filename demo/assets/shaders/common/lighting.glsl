#ifndef COMMON_LIGHTING_GLSL
#define COMMON_LIGHTING_GLSL

#include "../common/material.glsl"

struct point_light {
  vec4 color;
  vec3 position;
  float radius;
}; // point_light

const uint MAX_POINT_LIGHTS = 16;

struct directional_light {
  vec3 direction;
  vec4 color;
}; // directional_light

vec4 calculate_point_light(point_light light, vec3 position, vec3 normal, vec4 color, float shininess) {
  vec3 light_direction = normalize(light.position - position);
  float light_distance = length(light.position - position);

  float attenuation = 1.0 / (1.0 + 0.1 * light_distance + 0.01 * light_distance * light_distance);
  vec3 half_vector = normalize(light_direction + normalize(position));

  float diffuse = max(dot(normal, light_direction), 0.0);
  float specular = pow(max(dot(normal, half_vector), 0.0), shininess);

  return attenuation * (color * light.color * diffuse + specular);
}

vec4 calculate_directional_light(directional_light light, vec3 position, vec3 normal, vec4 color, float shininess) {
  vec3 light_direction = normalize(light.direction);
  vec3 half_vector = normalize(light_direction + normalize(position));

  float diffuse = max(dot(normal, light_direction), 0.0);
  float specular = pow(max(dot(normal, half_vector), 0.0), shininess);

  return color * light.color * diffuse + specular;
}

#endif // COMMON_LIGHTING_GLSL
