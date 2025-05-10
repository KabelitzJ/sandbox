#version 450 core

#include <libsbx/common/lighting.glsl>
#include <libsbx/common/material.glsl>
#include <libsbx/common/random.glsl>
#include <libsbx/common/depth.glsl>

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  vec2 resolution;
} scene;

layout(binding = 2) uniform sampler2D depth_image;
layout(binding = 3) uniform sampler2D texture_image;

float rim_factor(vec3 view_direction, vec3 normal, float strength) {
  float factor = 1.0 - abs(dot(view_direction, normal));

  return pow(factor, strength);
}

void main(void) {
  vec2 screen_uv = gl_FragCoord.xy / scene.resolution;
  float scene_depth = texture(depth_image, screen_uv).r;

  float current_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);

  if (current_depth > scene_depth) {
    discard;
  }

  vec3 view_direction = normalize(scene.camera_position - in_position);

  float difference = abs(current_depth - scene_depth);

  float threshold = 0.0002;

  float normalized_difference = clamp(difference / threshold, 0.0, 1.0);

  vec4 intersection = mix(vec4(1.0), vec4(0.0), normalized_difference);

  float rim_intensity = rim_factor(view_direction, in_normal, 4.0);

  float hex = texture(texture_image, in_uv).r;

  out_color = vec4(in_color.rgb, 0.1) + (intersection + vec4(rim_intensity)) * hex;
}
