#include <libsbx/common/constants.slang>

struct vertex {
  float position_x;
  float position_y;
  float position_z;
}; // struct vertex

struct push_data {
  Ptr<vertex> vertices;
}; // struct push_data

[[vk::push_constant]]
ConstantBuffer<push_data> push;

struct scene_data {
  float4x4 model;
  float4x4 view;
  float4x4 projection;
  float4 tint;
}; // struct scene_data

[[vk::binding(0, 0)]]
ConstantBuffer<scene_data> scene;

struct vs_in {
  uint vertex_id : SV_VertexID;
}; // struct vs_in

struct vs_out {
  [[vk::location(0)]]
  float3 uv : UV;
  [[vk::location(1)]]
  float4 color : COLOR;
  float4 position : SV_Position;
}; // struct vs_out

float3 position_from_vertex(vertex v) {
  return float3(v.position_x, v.position_y, v.position_z);
}

[shader("vertex")]
vs_out vs_main(vs_in in) {
  vs_out out;

  vertex vertex = push.vertices[in.vertex_id];
  float3 in_position = position_from_vertex(vertex);

  float3 view_rotation = mul((float3x3)scene.view, in_position);

  float4 clip = mul(scene.projection, float4(view_rotation, 1.0));
  out.position = float4(clip.x, clip.y, clip.w, clip.w);

  // pass-through
  out.uv = in_position;
  out.color = scene.tint;

  return out;
}
