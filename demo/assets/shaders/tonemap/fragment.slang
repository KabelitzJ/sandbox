#include <libsbx/math/constants.slang>

[[vk::binding(0, 0)]] 
Sampler2D resolve_image;

[[vk::binding(1, 0)]] 
Sampler2D bloom_image;

struct push_data {
  float exposure;
  float bloom_mix;
}; // struct push_data

[[vk::push_constant]]
ConstantBuffer<push_data> push;

struct fs_in {
  float2 uv : TEXCOORD0;
}; // struct fs_in

struct fs_out {
  float4 color : SV_Target0;
}; // struct fs_out

float3 aces_film(float3 x) {
  float a = 2.51;
  float b = 0.03;
  float c = 2.43;
  float d = 0.59;
  float e = 0.14;

  return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  const float gamma = 2.2;

  float3 color = resolve_image.Sample(in.uv).rgb;
  float3 bloom = bloom_image.Sample(in.uv).rgb;

  float3 hdr = color + bloom * push.bloom_mix;

  float3 mapped = aces_film(hdr * push.exposure);

  out.color = float4(mapped, 1.0);

  return out;
}
