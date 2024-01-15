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

vec4 calculate_directional_light_blinn_phong(material mat, directional_light light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  vec3 halfDir = normalize(lightDir + viewDir);

  float ambientStrength = 0.3; // You can adjust this based on your scene
  vec4 ambient = mat.ambient * ambientStrength;

  float diffuseStrength = max(dot(normal, lightDir), 0.0);
  vec4 diffuse = mat.diffuse * light.color * diffuseStrength;

  float specularStrength = pow(max(dot(normal, halfDir), 0.0), mat.shininess);
  vec4 specular = mat.specular * light.color * specularStrength;

  return ambient + diffuse + specular;
}

#endif // COMMON_LIGHTING_GLSL
