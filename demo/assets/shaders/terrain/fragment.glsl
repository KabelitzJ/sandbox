#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"
#include "../common/depth.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;
layout(location = 3) out float out_depth;

layout(binding = 2) uniform sampler2D grass_albedo_image;
layout(binding = 3) uniform sampler2D grass_normal_image;
layout(binding = 4) uniform sampler2D dirt_albedo_image;
layout(binding = 5) uniform sampler2D dirt_normal_image;

// vec3 get_normal_from_map(sampler2D image) {
//   vec3 tangent_normal = texture(image, in_uv).xyz * 2.0 - 1.0;
// 
//   vec3 Q1  = dFdx(in_position);
//   vec3 Q2  = dFdy(in_position);
//   vec2 st1 = dFdx(in_uv);
//   vec2 st2 = dFdy(in_uv);
// 
//   vec3 N   = normalize(in_normal);
//   vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
//   vec3 B  = -normalize(cross(N, T));
//   mat3 TBN = mat3(T, B, N);
// 
//   return normalize(TBN * tangent_normal);
// }

// uvec4 ihash1d(uvec4 q) {
//   q = q * 747796405u + 2891336453u;
//   q = (q << 13u) ^ q;
//   return q * (q * q * 15731u + 789221u) + 1376312589u;
// }
// 
// vec4 multi_hash2d(vec4 cell) {
//   uvec4 i = uvec4(cell);
//   uvec4 hash = ihash1d(ihash1d(i.xzxz) + i.yyww);
//   return vec4(hash) * (1.0 / float(0xffffffffu));
// }
// 
// vec2 noise_interpolate(vec2 x) { 
//   vec2 x2 = x * x;
//   return x2 * x * (x * (x * 6.0 - 15.0) + 10.0); 
// }

// float noise(vec2 pos, vec2 scale, float phase, float seed) {
//   const float kPI2 = 6.2831853071;
//   pos *= scale;
//   vec4 i = floor(pos).xyxy + vec2(0.0, 1.0).xxyy;
//   vec2 f = pos - i.xy;
//   i = mod(i, scale.xyxy) + seed;
// 
//   vec4 hash = multi_hash2d(i);
//   hash = 0.5 * sin(phase + kPI2 * hash) + 0.5;
//   float a = hash.x;
//   float b = hash.y;
//   float c = hash.z;
//   float d = hash.w;
// 
//   vec2 u = noise_interpolate(f);
//   float value = mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
//   return value * 2.0 - 1.0;
// }

// float fbm(vec2 pos, vec2 scale, int octaves, float shift, float timeShift, float gain, float lacunarity, float octaveFactor, float seed) {
//   float amplitude = gain;
//   float time = timeShift;
//   vec2 frequency = scale;
//   vec2 offset = vec2(shift, 0.0);
//   vec2 p = pos * frequency;
//   octaveFactor = 1.0 + octaveFactor * 0.12;
//   
//   vec2 sinCos = vec2(sin(shift), cos(shift));
//   mat2 rotate = mat2(sinCos.y, sinCos.x, sinCos.x, sinCos.y);
// 
//   float value = 0.0;
// 
//   for (int i = 0; i < octaves; i++) {
//     float n = noise(p / frequency, frequency, time, seed);
//     value += amplitude * n;
//     
//     p = p * lacunarity + offset * float(1 + i);
//     frequency *= lacunarity;
//     amplitude = pow(amplitude * gain, octaveFactor);
//     time += timeShift;
//     offset *= rotate;
//   }
// 
//   return value * 0.5 + 0.5;
// }

void main(void) {
  out_position = vec4(in_position, 1.0);

  // float noise = clamp(fbm(in_uv / 20.0, vec2(8.0), 16, 0.0, 0.0, 0.5, 2.0, -0.5, 8502384.237552), 0.0, 1.0);

  vec4 grass_albedo = texture(grass_albedo_image, in_uv);
  vec4 dirt_albedo = texture(dirt_albedo_image, in_uv);

  // out_albedo = mix(grass_albedo, dirt_albedo, noise);
  // out_albedo = vec4(noise, noise, noise, 1.0);
  out_albedo = grass_albedo;

  // vec4 grass_normal = vec4(get_normal_from_map(grass_normal_image), 1.0);
  // vec4 dirt_normal = vec4(get_normal_from_map(dirt_normal_image), 1.0);

  // out_normal = mix(grass_normal, dirt_normal, noise);
  out_normal = vec4(in_normal, 1.0);

  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
