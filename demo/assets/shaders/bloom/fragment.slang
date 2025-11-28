#include <libsbx/math/constants.slang>
#include <libsbx/math/algorithm.slang>

[[vk::binding(0, 0)]]
Sampler2D image;

struct push_data {
  float threshold;   // 1.0 good default
  float intensity;   // 1.0 good default
  float radius;      // 8–12 recommended
};

[[vk::push_constant]]
ConstantBuffer<push_data> push;

struct fs_in {
    float2 uv : TEXCOORD0;
};

struct fs_out {
    float4 color : SV_Target0;
};

// ====================================================================================
// FINAL BLOOM PASS
// ====================================================================================
[shader("fragment")]
fs_out main(fs_in input)
{
  fs_out out;

  uint w, h;
  image.GetDimensions(w, h);

  float2 texel = 1.0 / float2(w, h);
  float2 r = 15.0f * texel;

  const float2 offsets[13] = {
    float2( 0.0,      0.0),
    float2(-r.x,      0.0), 
    float2( r.x,      0.0),
    float2( 0.0,     -r.y), 
    float2( 0.0,      r.y),
    float2(-0.5*r.x, -0.866*r.y), 
    float2( 0.5*r.x, -0.866*r.y),
    float2(-0.5*r.x,  0.866*r.y), 
    float2( 0.5*r.x,  0.866*r.y),
    float2(-2.0*r.x,  0.0),       
    float2( 2.0*r.x,  0.0),
    float2( 0.0,     -2.0*r.y),   
    float2( 0.0,      2.0*r.y)
  };

  const float weights[13] = {
    0.20,
    0.10, 0.10,
    0.10, 0.10,
    0.07, 0.07, 0.07, 0.07,
    0.03, 0.03, 0.03, 0.03
  };

  float3 sum = 0.0;
  float weight_sum = 0.0;

  [unroll]
  for (int i = 0; i < 13; ++i) {
    float3 hdr = image.Sample(input.uv + offsets[i]).rgb;

    float3 diff  = max(hdr - push.threshold, float3(0.0));
    float3 bloom = diff * diff;
    bloom *= push.intensity;

    float weight = weights[i];

    sum += bloom * weight;
    weight_sum += weight;
  }

  sum /= weight_sum;

  out.color = float4(sum, 1.0);
  return out;
}
