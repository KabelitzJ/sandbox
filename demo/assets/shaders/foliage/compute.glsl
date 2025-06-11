#version 460 core

#extension GL_EXT_buffer_reference : enable

#include <libsbx/common/vk.glsl>

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct grass_blade {
	vec4 position_bend;         // xyz = position, w = bend amount
	vec4 size_animation_pitch;  // x = width, y = height, z = pitch angle, w = animation term
};

layout(buffer_reference, std430) readonly buffer grass_input_reference {
  grass_blade data[];
};

layout(buffer_reference, std430) writeonly buffer grass_output_reference {
  grass_blade data[];
};

layout(buffer_reference, std430) buffer draw_command_reference {
  vk_draw_indirect_command command;
};

layout(push_constant) uniform push_constants {
  grass_input_reference in_blades;
  grass_output_reference out_blades;
  draw_command_reference draw_command;
  mat4 view_projection;
  uint blade_count;
};

void main() {
  uint idx = gl_GlobalInvocationID.x;

  if (idx >= blade_count) {
    return;
  }

  grass_blade blade = in_blades.data[idx];
  vec3 world_position = blade.position_bend.xyz;

  vec4 clip = view_projection * vec4(world_position, 1.0);

  if (abs(clip.x) > clip.w || abs(clip.y) > clip.w || clip.z < 0.0 || clip.z > clip.w) {
    return;
  }

  uint output_idx = atomicAdd(draw_command.command.instance_count, 1);

  out_blades.data[output_idx] = blade;
}
