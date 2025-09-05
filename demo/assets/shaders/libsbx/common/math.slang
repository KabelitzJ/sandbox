#ifndef LIBSBX_COMMON_MATH_GLSL_
#define LIBSBX_COMMON_MATH_GLSL_

float log10(float x) {
  return log(x) / log(10.0);
}

float satf(float x) {
  return clamp(x, 0.0, 1.0);
}

float2 satv(float2 x) {
 return clamp(x, float2(0.0), float2(1.0));
}

float max2(float2 v) {
  return max(v.x, v.y);
}

#endif // LIBSBX_COMMON_MATH_GLSL_
