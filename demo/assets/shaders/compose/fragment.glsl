#version 460 core

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(set = 0, binding = 2) uniform sampler2D accumulation_image;
layout(set = 0, binding = 3) uniform sampler2D revealage_image; 

float max_component(vec4 v) {
  return max(max(v.x, v.y), max(v.z, v.w));
}

void main() {
  ivec2 texel = ivec2(gl_FragCoord.xy);

  vec4 accumulation = texelFetch(accumulation_image, texel, 0);
  float revealage = accumulation.a;

  accumulation.a = texelFetch(revealage_image, texel, 0).r;

  // suppress underflow
  if (isinf(accumulation.a)) {
    accumulation.a = max(max(accumulation.r, accumulation.g), accumulation.b);
  }

  // suppress overflow
  if (any(isinf(accumulation.rgb))) {
    accumulation = vec4(isinf(accumulation.a) ? 1.0 : accumulation.a);
  }

  vec3 average_color = accumulation.rgb / max(accumulation.a, 1e-4);

  // dst' =  (accumulation.rgb / accumulation.a) * (1 - revealage) + dst * revealage
  out_color = vec4(average_color, revealage);
}
