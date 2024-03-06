#ifndef COMMON_SHADOW_GLSL_
#define COMMON_SHADOW_GLSL_

float calculate_shadow(sampler2D shadow_map, vec4 light_space_position, vec3 normal, vec3 light_direction) {
  float shadow = 0.0;

  vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

  vec3 coordinates = light_space_position.xyz / light_space_position.w;

  if (coordinates.z > 1.0) {
    return shadow;
  }

  // float bias = max(0.001 * (1.0 - dot(normal, light_direction)), 0.0001);
  float bias = 0.001;
  
  float current_depth = coordinates.z - bias;

  int count = 0;
  int range = 2;

  for (int x = -range; x <= range; ++x) {
    for (int y = -range; y <= range; ++y) {
      float pcf_depth = texture(shadow_map, coordinates.xy + vec2(x, y) * texel_size).r;
      shadow += current_depth > pcf_depth ? 1.0 : 0.0;
      ++count;
    }
  }

  return shadow / float(count);
}

#endif // COMMON_SHADOW_GLSL_
