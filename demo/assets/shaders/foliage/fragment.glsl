#version 460 core

layout(location = 0) in vec2 in_uv;
layout(location = 1) in float in_wind;

layout(location = 0) out vec4 out_color;

void main() {
	// Simple alpha fade near the top
	float alpha = smoothstep(1.0, 0.6, in_uv.y);

	// Optional: color shifts with wind for shimmer
	vec3 base_color = mix(vec3(0.2, 0.5, 0.2), vec3(0.3, 0.7, 0.3), in_wind * 0.5 + 0.5);

	out_color = vec4(base_color, alpha);

	if (out_color.a < 0.1) {
		discard;
	}
}
