struct cs_in {
  uint3 id : SV_DispatchThreadID;
}; // struct cs_in

[[vk::binding(0, 0)]]
RWTexture2DArray<float4> outColour;

[[vk::binding(1, 0)]]
TextureCube<float4> skyboxTex;

[[vk::binding(2, 0)]]
SamplerState skyboxSampler;

static const float PI = 3.1415926535897932384626433832795;
static const float deltaPhi = (2.0 * PI) / 180.0;
static const float deltaTheta = (0.5 * PI) / 64.0;

float3 cubeDir(float2 texCoord, uint side) {
  float2 tex = texCoord * 2.0 - 1.0;

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
  outColour.GetDimensions(width, height, layers);

  float2 uv = (float2(in.id.xy) + float2(0.5)) / float2(width, height);

  for (uint face = 0; face < 6; ++face) {
    float3 N = normalize(cubeDir(uv, face));
    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, N));
    up = cross(N, right);

    const float TWO_PI = PI * 2.0;
    const float HALF_PI = PI * 0.5;

    float3 colour = float3(0.0, 0.0, 0.0);
    uint sampleCount = 0;

    for (float phi = 0.0; phi < TWO_PI; phi += deltaPhi) {
      for (float theta = 0.0; theta < HALF_PI; theta += deltaTheta) {
        float3 tempVec = cos(phi) * right + sin(phi) * up;
        float3 sampleVec = cos(theta) * N + sin(theta) * tempVec;

        float weight = cos(theta) * sin(theta);

        float3 sampleColor =
          skyboxTex.SampleLevel(skyboxSampler, sampleVec, 0.0).rgb;

        colour += sampleColor * weight;
        sampleCount++;
      }
    }

    float3 finalColour = PI * colour / float(sampleCount);

    outColour[int3(in.id.xy, face)] = float4(finalColour, 1.0);
  }
}
