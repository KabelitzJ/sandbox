#version 460 core

#extension GL_EXT_buffer_reference : enable

#include <foliage/grass_blade.glsl>

layout(location = 0) out vec2 out_uv;
layout(location = 1) out float out_wind;

layout(push_constant) uniform push_constants {
	grass_buffer_reference blades;
	mat4 view_projection;
	vec4 green_bottom;
	vec4 green_top;
	float global_time;
};

vec2 triangle_vertices[3] = vec2[](
	vec2( 0.0, 1.0),  // tip
	vec2(-0.5, 0.0),  // left base
	vec2( 0.5, 0.0)   // right base
);

void main() {
	uint vertex_id = uint(gl_VertexIndex);
	uint blade_id  = uint(gl_InstanceIndex);

	grass_blade blade = blades.data[blade_id];

	vec3 base_position = blade.position_bend.xyz;
	float bend = blade.position_bend.w;
	float width = blade.size_animation_pitch.x;
	float height = blade.size_animation_pitch.y;
	float animation = blade.size_animation_pitch.z;
	float pitch = blade.size_animation_pitch.w;

	vec2 uv = triangle_vertices[vertex_id];
	vec3 offset = vec3(uv.x * width, uv.y * height, 0.0);

	// Apply pitch rotation (around Y axis)
	float cos_pitch = cos(pitch);
	float sin_pitch = sin(pitch);

	offset = vec3(
		offset.x * cos_pitch + offset.z * sin_pitch,
		offset.y,
	  -offset.x * sin_pitch + offset.z * cos_pitch
	);

	// Apply wind sway
	float sway = sin(global_time + animation) * bend;
	offset.x += offset.y * sway;

	vec3 world_position = base_position + offset;

	gl_Position = view_projection * vec4(world_position, 1.0);

	out_uv = uv;
	out_wind = sway;
}
