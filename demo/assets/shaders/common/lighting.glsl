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

struct light_result {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
}; // struct light_result

light_result calculate_directional_light_blinn_phong(material material, directional_light light, vec3 normal, vec3 view_direction) {
  light_result result;

  vec3 light_direction = normalize(-light.direction);
  vec3 half_direction = normalize(light_direction + view_direction);

  float ambient_strength = 0.3; // you can adjust this based on your scene
  result.ambient = material.ambient * ambient_strength;

  float diffuse_strength = max(dot(normal, light_direction), 0.0);
  result.diffuse = material.diffuse * light.color * diffuse_strength;

  float specular_strength = pow(max(dot(normal, half_direction), 0.0), material.shininess);
  result.specular = material.specular * light.color * specular_strength;

  return result;
}

vec4 quantize(vec4 color, float levels) {
  return floor(color * levels) / levels;
}

light_result calculate_cell_shading(material material, directional_light light, vec3 normal, vec3 view_direction) {
  light_result result;
    
  // Normalize the input vectors
  vec3 N = normalize(normal);
  vec3 L = normalize(light.direction);
  vec3 V = normalize(view_direction);
  vec3 R = normalize(reflect(L, N));
  
  float specular = max(dot(R, V), 0.0);
  float diffuse = max(dot(N, L), 0.0);

  float intensity = 0.6 * diffuse + 0.4 * specular;

  if (intensity > 0.95) {
    result.ambient = material.ambient;
    result.diffuse = material.diffuse;
    result.specular = material.specular;
  } else if (intensity > 0.5) {
    result.ambient = material.ambient;
    result.diffuse = material.diffuse * 0.5;
    result.specular = material.specular * 0.5;
  } else if (intensity > 0.25) {
    result.ambient = material.ambient;
    result.diffuse = material.diffuse * 0.25;
    result.specular = material.specular * 0.25;
  } else {
    result.ambient = material.ambient;
    result.diffuse = material.diffuse * 0.1;
    result.specular = material.specular * 0.1;
  }
  
  return result;
}

#endif // COMMON_LIGHTING_GLSL
