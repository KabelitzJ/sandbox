#version 460 core
#extension GL_EXT_buffer_reference : enable

#include <foliage/grass_blade.glsl>

layout(location = 0) in vec2 in_uv;
layout(location = 1) in float in_wind;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform push_constants {
	grass_buffer_reference blades;
	mat4 view_projection;
	vec4 green_bottom;
	vec4 green_top;
	float global_time;
};

void main() {
	// Alpha fade near tip
	float fade = smoothstep(0.6, 1.0, in_uv.y);
	float alpha = 1.0 - fade;

	// Color gradient from bottom to top
	vec3 gradient_color = mix(green_bottom.rgb, green_top.rgb, in_uv.y);

	// Add wind shimmer
	vec3 shimmer_color = mix(gradient_color, gradient_color * 1.2, in_wind * 0.5 + 0.5);

	// Fake shadow near base
	float shadow = smoothstep(0.0, 0.4, in_uv.y);
	vec3 shaded_color = shimmer_color * mix(0.6, 1.0, shadow);

	out_color = vec4(shaded_color, alpha);

	if (alpha < 0.1) {
		discard;
	}
}
