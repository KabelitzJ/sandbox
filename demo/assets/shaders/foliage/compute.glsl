#version 460 core

#extension GL_EXT_buffer_reference : enable

#include <libsbx/common/vk.glsl>
#include <foliage/grass_blade.glsl>

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(buffer_reference, std430) readonly buffer grass_input_reference {
  grass_blade data[];
};

layout(buffer_reference, std430) writeonly buffer grass_output_reference {
  grass_blade data[];
};

layout(buffer_reference, std430) buffer draw_command_reference {
  uint vertex_count;
  uint instance_count;
  uint first_vertex;
  uint first_instance;
};

layout(push_constant) uniform push_constants {
  grass_input_reference in_blades;
  grass_output_reference out_blades;
  draw_command_reference draw_command;
  mat4 view_projection;
  vec4 camera_position; // .xyz = world-space position, .w = optional max render distance
  uint blade_count;
};

void main() {
  uint idx = gl_GlobalInvocationID.x;

  if (idx >= blade_count) {
    return;
  }

  grass_blade blade = in_blades.data[idx];
  vec3 world_position = blade.position_bend.xyz;

  // --- Frustum culling ---
  vec4 clip = view_projection * vec4(world_position, 1.0);

  if (abs(clip.x) > clip.w || abs(clip.y) > clip.w || clip.z < 0.0 || clip.z > clip.w) {
    return;
  }

  // --- Distance culling ---
  float distance = length(world_position - camera_position.xyz);

  if (camera_position.w > 0.0 && distance > camera_position.w) {
    return;
  }

  // --- Backface-like culling (optional) ---
  vec3 to_camera = normalize(camera_position.xyz - world_position);
  float facing = dot(to_camera, vec3(0.0, 1.0, 0.0)); // blade up = +Y
  
  if (facing < 0.1) {
    return;
  }

  // --- Write visible blade ---
  uint output_idx = atomicAdd(draw_command.instance_count, 1);
  out_blades.data[output_idx] = blade;
}
