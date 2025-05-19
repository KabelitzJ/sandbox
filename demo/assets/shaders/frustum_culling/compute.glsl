#version 450 core

#extension GL_EXT_buffer_reference: enable

#include <libsbx/math/aabb.glsl>

struct draw_indexed_indirect_command {
  uint count;
  uint instance_count;
  uint first_index;
  int base_vertex;
  uint base_instance;
}; // struct draw_indexed_indirect_command

struct draw_data {
  uint transform_id;
  uint material_id;
}; // struct draw_data

layout(local_size_x = 32) in;

layout(std430, buffer_reference) readonly buffer bounding_box_buffer {
  aabb data[];
};

layout(std430, buffer_reference) readonly buffer draw_data_buffer {
  draw_data data[];
};

layout(std430, buffer_reference) buffer draw_command_buffer {
  draw_indexed_indirect_command data[];
};

layout(std430, buffer_reference) buffer frustum_buffer {
  vec4 planes[6];
  vec4 corners[8];
  uint num_meshes_to_cull;
  uint num_visible_meshes;
};

layout(push_constant) uniform push_data {
  draw_command_buffer draw_commands;
  draw_data_buffer draw_data;
  bounding_box_buffer bounding_boxes;
  frustum_buffer frustum;
} push;

bool is_aabb_in_frustum(aabb box) {
  const vec3 min = aabb_get_min(box);
  const vec3 max = aabb_get_max(box);

  int r = 0;

  for (int i = 0; i < 6; i++) {
    r = 0;

    r += (dot(push.frustum.planes[i], vec4(min.x, min.y, min.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(max.x, min.y, min.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(min.x, max.y, min.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(max.x, max.y, min.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(min.x, min.y, max.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(max.x, min.y, max.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(min.x, max.y, max.z, 1.0f)) < 0) ? 1 : 0;
    r += (dot(push.frustum.planes[i], vec4(max.x, max.y, max.z, 1.0f)) < 0) ? 1 : 0;

    if (r == 8) {
      return false;
    }
  }

  r = 0; 

  for ( int i = 0; i < 8; i++ ) {
    r += ((push.frustum.corners[i].x > max.x) ? 1 : 0);
  }

  if (r == 8)  {
    return false;
  }

  r = 0; 
  
  for ( int i = 0; i < 8; i++ ) {
    r += ((push.frustum.corners[i].x < min.x) ? 1 : 0);
  }

  if ( r == 8 ) {
    return false;
  }

  r = 0; 
  
  for ( int i = 0; i < 8; i++ ) {
    r += ((push.frustum.corners[i].y > max.y) ? 1 : 0);
  }

  if ( r == 8 ) {
    return false;
  }

  r = 0; 
  
  for (int i = 0; i < 8; i++) {
    r += ((push.frustum.corners[i].y < min.y) ? 1 : 0);
  }

  if ( r == 8 ) {
    return false;
  }

  r = 0; 
  
  for (int i = 0; i < 8; i++) {
    r += ((push.frustum.corners[i].z > max.z) ? 1 : 0);
  }

  if ( r == 8 ) {
    return false;
  }
  r = 0; 
  
  for (int i = 0; i < 8; i++) {
    r += ((push.frustum.corners[i].z < min.z) ? 1 : 0);
  }

  if ( r == 8 ) {
    return false;
  }

  return true;
}

void main() {
  const uint idx = gl_GlobalInvocationID.x;

  if (idx < push.frustum.num_meshes_to_cull) {
    const uint base_instance = push.draw_commands.data[idx].base_instance;

    const aabb box = push.bounding_boxes.data[push.draw_data.data[base_instance].transform_id];

    const uint num_instances = is_aabb_in_frustum(box) ? 1 : 0;

    push.draw_commands.data[idx].instance_count = num_instances;

    atomicAdd(push.frustum.num_visible_meshes, num_instances);
  }
}
