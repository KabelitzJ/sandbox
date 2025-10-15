#ifndef ALPHA_POLICY_SLANG_
#define ALPHA_POLICY_SLANG_

#include <features.slang>

interface alpha_policy {
  bool should_discard(float alpha, float alpha_cutoff);
  float4 finalize_color(float3 rgb, float alpha);
}; // interface alpha_policy

struct opaque_alpha_policy : alpha_policy {
  bool should_discard(float a, float cutoff) { return false; }
  float4 finalize_color(float3 rgb, float a) { return float4(rgb, 1.0); }
}; // struct opaque_alpha_policy

struct clip_alpha_policy : alpha_policy {
  bool should_discard(float a, float cutoff) { return a < cutoff; }
  float4 finalize_color(float3 rgb, float a) { return float4(rgb, 1.0); }
}; // struct clip_alpha_policy

#if SBX_FEATURE_ALPHA_CLIP
typedef clip_alpha_policy default_alpha_policy;
#else
typedef opaque_alpha_policy default_alpha_policy;
#endif

#endif // ALPHA_POLICY_SLANG_