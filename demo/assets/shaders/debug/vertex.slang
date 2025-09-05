struct vertex {
  float4 position;
  float4 color;
}; // struct vertex

struct push_data {
  float4x4 mvp;
  Ptr<vertex> vertices;
}; // struct push_data

[[vk::push_constant]]
ConstantBuffer<push_data> push;

struct vs_in { 
  uint vertex_id : SV_VertexID; 
}; // struct vs_in

struct vs_out {
  float4 color : TEXCOORD0; 
  float4 position : SV_Position;
}; // struct vs_out

[shader("vertex")]
vs_out vs_main(vs_in in) {
  vs_out out;

  vertex vertex = push.vertices[in.vertex_id];

  out.color = vertex.color;
  out.position = mul(push.mvp, vertex.position);

  return out;
}