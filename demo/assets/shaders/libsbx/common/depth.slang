#ifndef COMMON_DEPTH_GLSL_
#define COMMON_DEPTH_GLSL_

namespace sbx::common {

static const float default_near = 0.1;
static const float default_far = 1000.0;

float linearize_depth(float depth, float near, float far) {
  float z_ndc = depth * 2.0 - 1.0;
  float z_eye = (2.0 * near * far) / ((far + near) - z_ndc * (far - near));

  return (z_eye - near) / (far - near);
}

} // namespace sbx::common

#endif // COMMON_DEPTH_GLSL_
