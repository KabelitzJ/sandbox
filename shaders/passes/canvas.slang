// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz

#include <descriptors.slang>

struct canvas_vertex {
  float4 position;
  float4 color;
  float2 uv;
  uint texture_index;
  uint padding;
}; // struct canvas_vertex

struct push_data {
  canvas_vertex* vertices;
  uint sampler_index;
}; // struct push_data

[[vk::push_constant]] ConstantBuffer<push_data> push;

struct vertex_input {
  uint vertex_id : SV_VertexID;
}; // struct vertex_input

struct vertex_output {
  [[vk::location(0)]] float4 color : TEXCOORD0;
  [[vk::location(1)]] float2 uv : TEXCOORD1;
  [[vk::location(2)]] nointerpolation uint texture_index : TEXTURE_INDEX;
  float4 sv_position : SV_Position;
}; // struct vertex_output

struct fragment_input {
  [[vk::location(0)]] float4 color : TEXCOORD0;
  [[vk::location(1)]] float2 uv : TEXCOORD1;
  [[vk::location(2)]] nointerpolation uint texture_index : TEXTURE_INDEX;
}; // struct fragment_input

struct fragment_output {
  float4 color : SV_Target0;
}; // struct fragment_output

[shader("vertex")]
vertex_output vertex_main(vertex_input input) {
  canvas_vertex vertex = push.vertices[input.vertex_id];

  vertex_output output;

  output.color = vertex.color;
  output.uv = vertex.uv;
  output.texture_index = vertex.texture_index;
  output.sv_position = vertex.position;

  return output;
}

[shader("fragment")]
fragment_output fragment_main(fragment_input input) {
  fragment_output output;

  output.color = input.color * textures[input.texture_index].Sample(samplers[push.sampler_index], input.uv);

  return output;
}

[shader("fragment")]
fragment_output fragment_main_text(fragment_input input) {
  float distance = textures[input.texture_index].Sample(samplers[push.sampler_index], input.uv).r;
  float width = fwidth(distance);
  float alpha = smoothstep(0.5 - width, 0.5 + width, distance);

  fragment_output output;

  output.color = float4(input.color.rgb, input.color.a * alpha);

  return output;
}
