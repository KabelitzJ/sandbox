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

struct blinn_phong_result {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
}; // struct blinn_phong_result

blinn_phong_result calculate_directional_light_blinn_phong(material material, directional_light light, vec3 normal, vec3 view_direction) {
  blinn_phong_result result;

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

#endif // COMMON_LIGHTING_GLSL
