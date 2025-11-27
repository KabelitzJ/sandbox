struct cs_in {
  uint3 id : SV_DispatchThreadID;
}; // struct cs_in

[[vk::binding(0, 0)]]
SamplerCube<float4> skybox;

[[vk::binding(1, 0)]]
RWTexture2DArray<float4> output;

static const float PI = 3.1415926535897932384626433832795;
static const float DELTA_PHI = (2.0 * PI) / 180.0;
static const float DELTA_THETA = (0.5 * PI) / 64.0;

float3 cube_direction(float2 uv, uint side) {
  float2 tex = uv * 2.0 - 1.0;

  if (side == 0) return float3( 1.0, -tex.y, -tex.x);
  if (side == 1) return float3(-1.0, -tex.y,  tex.x);
  if (side == 2) return float3( tex.x,  1.0,  tex.y);
  if (side == 3) return float3( tex.x, -1.0, -tex.y);
  if (side == 4) return float3( tex.x, -tex.y,  1.0);
  if (side == 5) return float3(-tex.x, -tex.y, -1.0);

  return float3(1.0, 1.0, 1.0);
}

[shader("compute")]
[numthreads(16, 16, 1)]
void main(cs_in in) {
  uint width, height, layers;
  output.GetDimensions(width, height, layers);

  float2 uv = (float2(in.id.xy) + float2(0.5)) / float2(width, height);

  for (uint face = 0; face < 6; ++face) {
    float3 N = normalize(cube_direction(uv, face));
    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, N));
    up = cross(N, right);

    const float TWO_PI = PI * 2.0;
    const float HALF_PI = PI * 0.5;

    float3 color = float3(0.0, 0.0, 0.0);
    uint sample_count = 0;

    for (float phi = 0.0; phi < TWO_PI; phi += DELTA_PHI) {
      for (float theta = 0.0; theta < HALF_PI; theta += DELTA_THETA) {
        float3 direction = cos(phi) * right + sin(phi) * up;
        float3 uav = cos(theta) * N + sin(theta) * direction;

        float weight = cos(theta) * sin(theta);

        float3 sampled = skybox.Sample(uav).rgb;

        color += sampled * weight;
        sample_count++;
      }
    }

    float3 final_output = PI * color / float(sample_count);

    output.Store(int3(in.id.xy, face), float4(final_output, 1.0));
  }
}
