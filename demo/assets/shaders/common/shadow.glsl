#ifndef COMMON_SHADOW_GLSL_
#define COMMON_SHADOW_GLSL_

#include "../common/random.glsl"

const int PCF_COUNT = 8;
const float RADIUS = 0.0004;
const float BIAS = 0.0005;
const float SAMPLE_WEIGHT = 1.0 / (PCF_COUNT * PCF_COUNT);
const vec2 OFFSET = vec2(0.5 - 0.5 * PCF_COUNT);

float calculate_shadow(sampler2D shadow_map, vec4 light_space_position, vec3 normal, vec3 light_direction) {
  vec3 coordinates = light_space_position.xyz / light_space_position.w;

  if (coordinates.z > 1.0 || coordinates.z < -1.0) {
    return 0.0;
  }

	float total = 0.0;

	for(int x = 0; x < PCF_COUNT; x++) {
    for(int y = 0; y < PCF_COUNT; y++) {
      vec2 sample_position = vec2(x, y) + OFFSET;
      vec2 jitter = random_2d(gl_FragCoord.xy + vec2(13278 * x, 321 * y));

      float object_depth = texture(shadow_map, coordinates.xy + (sample_position + jitter) * RADIUS).r;

      float factor = (coordinates.z - BIAS) > object_depth ? 0.0 : 1.0;

      total += factor * SAMPLE_WEIGHT;
    }
	}

	return total;
}

// float calculate_shadow(sampler2D shadow_map, vec4 light_space_position, vec3 normal, vec3 light_direction) {
//   float shadow = 0.0;

//   vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

//   vec3 coordinates = light_space_position.xyz / light_space_position.w;

//   if (coordinates.z > 1.0) {
//     return shadow;
//   }

//   // float bias = max(0.001 * (1.0 - dot(normal, light_direction)), 0.0001);
//   float bias = 0.001;
  
//   float current_depth = coordinates.z - bias;

//   int count = 0;
//   int range = 2;

//   for (int x = -range; x <= range; ++x) {
//     for (int y = -range; y <= range; ++y) {
//       float pcf_depth = texture(shadow_map, coordinates.xy + vec2(x, y) * texel_size).r;
//       shadow += current_depth > pcf_depth ? 1.0 : 0.0;
//       ++count;
//     }
//   }

//   return shadow / float(count);
// }

#endif // COMMON_SHADOW_GLSL_
