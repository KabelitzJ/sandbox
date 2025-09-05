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

struct VSIn { 
  uint vertex_id : SV_VertexID; 
};

struct VSOut {
  float4 color : TEXCOORD0; 
  float4 position : SV_Position;
};

[shader("vertex")]
VSOut VSMain(VSIn IN) {
  VSOut OUT;

  vertex vertex = push.vertices[IN.vertex_id];

  OUT.color = vertex.color;
  OUT.position = mul(push.mvp, vertex.position);

  return OUT;
}