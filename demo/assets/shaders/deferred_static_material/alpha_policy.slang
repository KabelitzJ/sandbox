#ifndef ALPHA_POLICY_SLANG_
#define ALPHA_POLICY_SLANG_

#include <features.slang>

interface alpha_policy {
  static bool should_discard(float alpha, float alpha_cutoff);
  static float4 combine_colors(float4 salmpled, float4 base);
}; // interface alpha_policy

struct opaque_alpha_policy : alpha_policy {

  static bool should_discard(float a, float cutoff) { 
    return false; 
  }

  static float4 combine_colors(float4 salmpled, float4 base) { 
    return float4(salmpled.rgb * base.rgb, 1.0);
  }

}; // struct opaque_alpha_policy

struct clip_alpha_policy : alpha_policy {

  static bool should_discard(float a, float cutoff) { 
    return a < cutoff; 
  }

  static float4 combine_colors(float4 salmpled, float4 base) { 
    return salmpled.rgb * base.rgb, salmpled.a; 
  }

}; // struct clip_alpha_policy

struct transparent_alpha_policy : alpha_policy {

  static bool should_discard(float a, float cutoff) { 
    return false; 
  }

  static float4 combine_colors(float4 salmpled, float4 base) { 
    return float4(salmpled * base); 
  }

}; // struct transparent_alpha_policy

#endif // ALPHA_POLICY_SLANG_