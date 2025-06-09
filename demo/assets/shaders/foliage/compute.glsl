#version 460 core

#extension GL_EXT_buffer_reference: enable

#include <libsbx/common/random.glsl>

struct draw_indexed_indirect_command {
  uint count;
  uint instance_count;
  uint first_index;
  int base_vertex;
  uint base_instance;
}; // struct draw_indexed_indirect_command

struct grass_blade {
	// holds position(displacement from the center) of the blade
	// and a value determining how much it will bend
	vec4 position_bend;
	// holds width and height multiplier
	// and pitch angle
	// and a term used for animation
	vec4 size_animation_pitch;
};

layout(local_size_x = 32) in;

layout(std430, buffer_reference) buffer draw_command_buffer {
  draw_indexed_indirect_command data[];
};

layout(push_constant) uniform push_data {
  draw_command_buffer draw_commands;
  vec3 center;
  vec2 size;
} push;

void main() {
  const uvec2 idx = gl_GlobalInvocationID.xy;

  vec3 position = push.center;

  vec3 random_offset = random_3d(uvec3(idx, 378294));

  position.x += (push.size.x / 2) - random_offset.x * push.size.x;
  position.z += (push.size.y / 2) - random_offset.y * push.size.y;

  vec2 bottom_left_corner = push.center.xz - (push.size.xy / 2.0);
  vec2 upper_right_corner = push.center.xz + (push.size.xy / 2.0);

  vec2 uv = position.xz / (upper_right_corner - bottom_left_corner);
}
