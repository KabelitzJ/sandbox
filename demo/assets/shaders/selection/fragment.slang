#include <libsbx/common/scene_data.slang>

[[vk::binding(0, 0)]]
ConstantBuffer<sbx::common::scene_data> scene;

[[vk::binding(1, 0)]] Sampler2D<float4> image;
[[vk::binding(2, 0)]] Sampler2D<uint>   object_id_image;
[[vk::binding(3, 0)]] Sampler2D<float>  normalized_depth_image;

struct push_data {
  float4 color;     // rgb = outline color, a = max intensity
  float  thickness; // world-space thickness in scene units (e.g. 0.05)
}; // struct push_data

[[vk::push_constant]]
ConstantBuffer<push_data> push;

struct fs_in {
  float2 uv : TEXCOORD0;
};

struct fs_out {
  float4 color : SV_Target0;
};

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  float4 base_color      = image.Sample(in.uv);
  uint   center_id       = object_id_image.Sample(in.uv);
  float  normalized_depth = normalized_depth_image.Sample(in.uv);

  // No object here -> no outline
  if (center_id == 0u) {
    out.color = base_color;
    return out;
  }

  // Reconstruct view-space distance from normalized [0..1] depth
  float z_eye = scene.camera_near + normalized_depth * (scene.camera_far - scene.camera_near);

  // pixels per unit at this depth (assuming scene.viewport.y is HEIGHT in pixels)
  float pixels_per_unit = (scene.viewport.y * 0.5) / tan(scene.camera_fov_radians * 0.5);
  pixels_per_unit /= max(z_eye, 0.0001);

  // Convert world-space thickness -> pixels
  float thickness_px = push.thickness * pixels_per_unit;

  // Clamp to avoid massive kernels very close to camera
  thickness_px = clamp(thickness_px, 1.0, 40.0);

  uint width, height;
  object_id_image.GetDimensions(width, height);
  float2 texel = 1.0 / float2(width, height);

  int radius = (int)ceil(thickness_px);

  const float INF = 1e9;
  float min_dist = INF;

  [unroll]
  for (int x = -radius; x <= radius; ++x) {
    [unroll]
    for (int y = -radius; y <= radius; ++y) {
      if (x == 0 && y == 0)
        continue;

      float2 offset_uv = in.uv + float2(x, y) * texel;
      offset_uv = clamp(offset_uv, float2(0.0, 0.0), float2(1.0, 1.0));

      uint neighbor_id = object_id_image.Sample(offset_uv);
      if (neighbor_id != center_id) {
        float dist = length(float2(x, y)); // in pixels
        min_dist = min(min_dist, dist);
      }
    }
  }

  if (min_dist == INF) {
    out.color = base_color;
    return out;
  }

  // Fade inward: full at the border (≈1 px), then fade until thickness_px
  float border_dist = 1.0;
  float d = max(min_dist - border_dist, 0.0);

  float t = d / max(thickness_px, 0.0001); // 0 at border, 1 at inner limit
  t = saturate(t);

  float strength = 1.0 - t;
  float alpha    = strength * push.color.a;

  float3 final_rgb = lerp(base_color.rgb, push.color.rgb, alpha);

  out.color = float4(final_rgb, base_color.a);

  // out.color = float4(float(center_id << 24), float(center_id << 16), float(center_id << 8), 1.0);

  // out.color = base_color;

  return out;
}
