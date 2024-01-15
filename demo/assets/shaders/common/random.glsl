#ifndef COMMON_RANDOM_GLSL_
#define COMMON_RANDOM_GLSL_

vec2 random_2d(vec2 seed) {
  seed = vec2(dot(seed,vec2(127.1 ,311.7)), dot(seed,vec2(269.5, 183.3)));

  return fract(sin(seed) * 43758.5453123);
}

#endif // COMMON_RANDOM_GLSL_
