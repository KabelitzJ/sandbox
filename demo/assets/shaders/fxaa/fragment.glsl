#version 450

layout(location = 0) in vec2 in_uv;

layout(binding = 1) uniform sampler2D image;

layout(location = 0) out vec4 out_color;

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;

void main() {
  vec2 resolution = vec2(textureSize(image, 0));

  vec2 inv_resolution = 1.0 / resolution;

  vec3 rgb_nw = texture(image, in_uv + vec2(-1.0, -1.0) * inv_resolution).xyz;
  vec3 rgb_ne = texture(image, in_uv + vec2(1.0, -1.0) * inv_resolution).xyz;
  vec3 rgb_sw = texture(image, in_uv + vec2(-1.0, 1.0) * inv_resolution).xyz;
  vec3 rgb_se = texture(image, in_uv + vec2(1.0, 1.0) * inv_resolution).xyz;
  vec3 rgb_m = texture(image, in_uv).xyz;

  vec3 luma = vec3(0.299, 0.587, 0.114);

  float luma_nw = dot(rgb_nw, luma);
  float luma_ne = dot(rgb_ne, luma);
  float luma_sw = dot(rgb_sw, luma);
  float luma_se = dot(rgb_se, luma);
  float luma_m = dot(rgb_m, luma);

  float luma_min = min(luma_m, min(min(luma_nw, luma_ne), min(luma_sw, luma_se)));
  float luma_max = max(luma_m, max(max(luma_nw, luma_ne), max(luma_sw, luma_se)));

  float luma_range = luma_max - luma_min;

  if (luma_range < max(EDGE_THRESHOLD_MIN, luma_max * EDGE_THRESHOLD_MAX)) {
    out_color = vec4(rgb_m, 1.0);
    return;
  }

  vec2 dir;

  dir.x = -((luma_nw + luma_ne) - (luma_sw + luma_se));
  dir.y = ((luma_nw + luma_sw) - (luma_ne + luma_se));

  float dir_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * (0.25 * 0.25), 0.001);

  float rcp_dir_min = 1.0 / (min(abs(dir.x), abs(dir.y)) + dir_reduce);

  dir = min(vec2(8.0, 8.0), max(vec2(-8.0, -8.0), dir * rcp_dir_min)) * inv_resolution;

  vec3 rgb_a = 0.5 * (texture(image, in_uv + dir * (1.0 / 3.0 - 0.5)).xyz + texture(image, in_uv + dir * (2.0 / 3.0 - 0.5)).xyz);

  vec3 rgb_b = rgb_a * 0.5 + 0.25 * (texture(image, in_uv + dir * -0.5).xyz + texture(image, in_uv + dir * 0.5).xyz);

  float luma_b = dot(rgb_b, luma);

  if ((luma_b < luma_min) || (luma_b > luma_max)) {
    out_color = vec4(rgb_a, 1.0);
  } else {
    out_color = vec4(rgb_b, 1.0);
  }
}
