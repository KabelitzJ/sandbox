#ifndef COMMON_MATERIAL_GLSL
#define COMMON_MATERIAL_GLSL

struct material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
}; // struct material

#endif // COMMON_MATERIAL_GLSL
