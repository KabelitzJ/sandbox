// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz

#include <common.slang>
#include <frame_data.slang>

struct push_data {
  frame_data* frame_address;
  transform_data* source_transforms;
  transform_data* destination_transforms;
  draw_indexed_indirect_command* indirect_args;
  float4 local_bounds_min;
  float4 local_bounds_max;
  uint command_index;
  uint transform_offset;
  uint instance_count;
  uint padding;
}; // struct push_data

[[vk::push_constant]]
ConstantBuffer<push_data> push;

struct compute_input {
  uint3 id : SV_DispatchThreadID;
}; // struct compute_input

bool aabb_in_frustum(frame_data frame, float3 center, float3 extent) {
  for (uint i = 0u; i < 6u; ++i) {
    float4 plane = frame.frustum_planes[i];
    float radius = extent.x * abs(plane.x) + extent.y * abs(plane.y) + extent.z * abs(plane.z);
    float distance = dot(plane.xyz, center) + plane.w;

    if (distance + radius < 0.0) {
      return false;
    }
  }

  return true;
}

[shader("compute")]
[numthreads(64, 1, 1)]
void compute_main(compute_input input) {
  uint instance_id = input.id.x;

  if (instance_id >= push.instance_count) {
    return;
  }

  transform_data transform_data = push.source_transforms[push.transform_offset + instance_id];

  float3 local_min = push.local_bounds_min.xyz;
  float3 local_max = push.local_bounds_max.xyz;

  float3 corners[8] = {
    float3(local_min.x, local_min.y, local_min.z),
    float3(local_min.x, local_min.y, local_max.z),
    float3(local_min.x, local_max.y, local_min.z),
    float3(local_min.x, local_max.y, local_max.z),
    float3(local_max.x, local_min.y, local_min.z),
    float3(local_max.x, local_min.y, local_max.z),
    float3(local_max.x, local_max.y, local_min.z),
    float3(local_max.x, local_max.y, local_max.z)
  };

  float3 world_min = float3(3.402823466e+38, 3.402823466e+38, 3.402823466e+38);
  float3 world_max = -world_min;

  for (uint i = 0u; i < 8u; ++i) {
    float3 world_corner = mul(transform_data.model, float4(corners[i], 1.0)).xyz;
    world_min = min(world_min, world_corner);
    world_max = max(world_max, world_corner);
  }

  float3 center = (world_min + world_max) * 0.5;
  float3 extent = (world_max - world_min) * 0.5;

  frame_data frame = push.frame_address[0];

  if (!aabb_in_frustum(frame, center, extent)) {
    return;
  }

  uint slot;
  InterlockedAdd(push.indirect_args[push.command_index].instance_count, 1u, slot);

  push.destination_transforms[push.transform_offset + slot].model = transform_data.model;
  push.destination_transforms[push.transform_offset + slot].normal = transform_data.normal;
}
