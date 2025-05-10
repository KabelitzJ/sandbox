#version 450 core

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"
#include "../common/depth.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_water_color;
layout(location = 4) in vec4 in_land_color;
layout(location = 5) in vec4 in_mountain_color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;
layout(location = 3) out float out_depth;

void main(void) {
  out_position = vec4(in_position, 1.0);

  out_albedo = in_land_color;
  out_normal = vec4(in_normal, 1.0);

  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
