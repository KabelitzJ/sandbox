struct fs_in {
  float3 uv : TEXCOORD0;
  float4 color : COLOR0;
}; // struct fs_in

struct fs_out {
  float4 color : SV_Target0;
}; // struct fs_out

[[vk::binding(1, 0)]]
SamplerCube skybox;

static const float FOG_DISTANCE = 400.0;
static const float FOG_DENSITY = 0.002;
static const float4 FOG_COLOR = float4(0.5, 0.5, 0.5, 1.0);

[shader("fragment")]
fs_out fs_main(fs_in in) {
  fs_out out;

  float3 direction = normalize(in.uv);
  float4 cube_map_color = skybox.Sample(direction) * in.color;

  float fog_factor = clamp(1.0 - exp(-FOG_DISTANCE * FOG_DENSITY), 0.0, 1.0);

  out.color = lerp(cube_map_color, FOG_COLOR, fog_factor);

  return out;
}
