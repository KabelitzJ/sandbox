#version 460 core

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : enable

#include <foliage/grass_blade.glsl>

// Declare fragment shader inputs
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform push_constants {
	grass_buffer_reference blades;
	mat4 model;
	mat4 view_projection;
	vec4 green_bottom;
	vec4 green_top;
	float global_time;
};

void main() {
  // Compute fragment color
  vec3 dark_green = vec3(0.2, 0.4, 0.0);
  vec3 light_green = vec3(0.4, 0.8, 0.0);
  vec3 light = normalize(vec3(1.0, 1.0, 1.0));

  // 1. compute color; mixing from dark to light green
  vec3 color = mix(dark_green, light_green, uv.y);

  // 2. compute lambertian shading
  float lambert_diffuse = dot(vec3(normal), light);
  float lambert_ambient = 0.4;
  float light_intensity = clamp(lambert_diffuse, 0, 1) + lambert_ambient;

  outColor = vec4(light_intensity * color, 1.0);
}
