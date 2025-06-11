#version 460 core

#extension GL_EXT_buffer_reference : enable

layout(location = 0) out vec2 out_uv;
layout(location = 1) out float out_wind;

struct grass_blade {
	vec4 position_bend;         // xyz = position, w = bend amount
	vec4 size_animation_pitch;  // x = width, y = height, z = pitch angle, w = animation term
};

layout(buffer_reference, std430) readonly buffer grass_reference {
	grass_blade data[];
};

layout(push_constant) uniform push_constants {
	grass_reference blades;
	mat4 view_projection;
	float global_time;
};

vec2 quad_vertices[6] = vec2[](
	vec2(-0.5, 0.0),
	vec2( 0.5, 0.0),
	vec2(-0.5, 1.0),
	vec2(-0.5, 1.0),
	vec2( 0.5, 0.0),
	vec2( 0.5, 1.0)
);

void main() {
	uint vertex_id = uint(gl_VertexIndex);
	uint blade_id  = uint(gl_InstanceIndex);

	grass_blade blade = blades.data[blade_id];

	vec3 base_position = blade.position_bend.xyz;
	float bend = blade.position_bend.w;

	float width  = blade.size_animation_pitch.x;
	float height = blade.size_animation_pitch.y;
	float pitch  = blade.size_animation_pitch.z;
	float animation   = blade.size_animation_pitch.w;

	vec2 quad_uv = quad_vertices[vertex_id];

	vec3 offset = vec3(quad_uv.x * width, quad_uv.y * height, 0.0);

	// Apply pitch rotation (around x-axis)
	float cos_pitch = cos(pitch);
	float sin_pitch = sin(pitch);

	offset = vec3(
		offset.x,
		offset.y * cos_pitch - offset.z * sin_pitch,
		offset.y * sin_pitch + offset.z * cos_pitch
	);

	// Apply wind sway
	float sway = sin(global_time + animation) * bend;
	offset.x += offset.y * sway;

	vec3 world_position = base_position + offset;

	gl_Position = view_projection * vec4(world_position, 1.0);

	out_uv = quad_uv;
	out_wind = sway;
}
