#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;

layout(binding = 2) uniform sampler2D image;

void main(void) {
  out_position = vec4(in_position, 1.0);
  out_normal = vec4(in_normal, 1.0);
  out_albedo = texture(image, in_uv) * in_color;
}
