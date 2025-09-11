#version 460 core

#include <libsbx/common/lighting.glsl>
#include <libsbx/common/shadow.glsl>
#include <libsbx/common/depth.glsl>
#include <libsbx/common/constants.glsl>

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_color;

// === Uniforms ===
layout(set = 0, binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(set = 0, binding = 1, std430) readonly buffer buffer_point_lights {
  point_light data[];
} point_lights;

// === G-buffer ===
layout(set = 0, binding = 2) uniform sampler2D albedo_image;
layout(set = 0, binding = 3) uniform sampler2D position_image;
layout(set = 0, binding = 4) uniform sampler2D normal_image;
layout(set = 0, binding = 5) uniform sampler2D material_image;
layout(set = 0, binding = 6) uniform usampler2D object_id_image;
layout(set = 0, binding = 7) uniform sampler2D shadow_image;

// === Constants ===
const vec3 DEFAULT_F0 = vec3(0.04);

mat4 BIAS_MATRIX = mat4(
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
);

// === Fresnel Schlick Approximation ===
vec3 fresnel_schlick(float cos_theta, vec3 f0) {
  return f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0);
}

// === Normal Distribution Function (GGX) ===
float distribution_ggx(vec3 n, vec3 h, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float ndh = max(dot(n, h), 0.0);
  float ndh2 = ndh * ndh;

  float denom = ndh2 * (a2 - 1.0) + 1.0;
  return a2 / (PI * denom * denom);
}

// === Geometry Function (Smith) ===
float geometry_schlick_ggx(float ndv, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  return ndv / (ndv * (1.0 - k) + k);
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness) {
  float ndv = max(dot(n, v), 0.0);
  float ndl = max(dot(n, l), 0.0);
  float ggx1 = geometry_schlick_ggx(ndv, roughness);
  float ggx2 = geometry_schlick_ggx(ndl, roughness);
  return ggx1 * ggx2;
}

// === Shadow Calculation ===
const vec2 POISSON_DISK[16] = vec2[](
  vec2(-0.94201624, -0.39906216),
  vec2( 0.94558609, -0.76890725),
  vec2(-0.094184101, -0.92938870),
  vec2( 0.34495938,  0.29387760),
  vec2(-0.91588581,  0.45771432),
  vec2(-0.81544232, -0.87912464),
  vec2(-0.38277543,  0.27676845),
  vec2( 0.97484398,  0.75648379),
  vec2( 0.44323325, -0.97511554),
  vec2( 0.53742981, -0.47373420),
  vec2(-0.26496911, -0.41893023),
  vec2( 0.79197514,  0.19090188),
  vec2(-0.24188840,  0.99706507),
  vec2(-0.81409955,  0.91437590),
  vec2( 0.19984126,  0.78641367),
  vec2( 0.14383161, -0.14100790)
);

float random2(vec2 seed) {
  return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

mat2 rotation_matrix(float angle) {
  float s = sin(angle);
  float c = cos(angle);

  return mat2(c, -s, s, c);
}

float calculate_shadow(vec3 world_position, vec3 normal) {
  vec4 shadow_coord = BIAS_MATRIX * scene.light_space * vec4(world_position, 1.0);
  vec3 projeted_coords = shadow_coord.xyz / shadow_coord.w;

  if (projeted_coords.z > 1.0) {
    return 1.0;
  }

  float bias = max(0.0005 * (1.0 - dot(normal, normalize(scene.light_direction))), 0.0001);
  vec2 texel_size = 1.0 / textureSize(shadow_image, 0);

  float distance_to_camera = length(scene.camera_position - world_position);
  float radius = mix(1.0, 4.0, clamp(distance_to_camera / 50.0, 0.0, 1.0));

  float angle = random2(projeted_coords.xy) * TWO_PI;
  mat2 rotation = rotation_matrix(angle);

  float shadow = 0.0;

  for (int i = 0; i < 16; ++i) {
    vec2 offset = rotation * POISSON_DISK[i];
    vec2 sample_uv = projeted_coords.xy + offset * texel_size * radius;
    float sample_depth = texture(shadow_image, sample_uv).r;

    shadow += (projeted_coords.z - bias > sample_depth) ? 0.0 : 1.0;
  }

  return shadow / 16.0;
}

void main() {
  // --- G-buffer sampling ---
  vec3 albedo = texture(albedo_image, in_uv).rgb;
  vec3 world_position = texture(position_image, in_uv).xyz;
  vec3 normal = normalize(texture(normal_image, in_uv).xyz);
  vec3 mrao = texture(material_image, in_uv).rgb;

  // out_color = vec4(albedo, 1.0);

  // return;

  float metallic = clamp(mrao.r, 0.0, 1.0);
  float roughness = clamp(mrao.g, 0.05, 1.0);
  float ao = clamp(mrao.b, 0.0, 1.0);

  vec3 view_dir = normalize(scene.camera_position - world_position);

  vec3 f0 = mix(DEFAULT_F0, albedo, metallic);

  vec3 color = vec3(0.0);

  // --- Directional Light ---
  vec3 light_dir = normalize(-scene.light_direction);
  vec3 halfway_dir = normalize(view_dir + light_dir);
  float ndl = max(dot(normal, light_dir), 0.0);

  float D = distribution_ggx(normal, halfway_dir, roughness);
  float G = geometry_smith(normal, view_dir, light_dir, roughness);
  vec3 F = fresnel_schlick(max(dot(halfway_dir, view_dir), 0.0), f0);

  vec3 numerator = D * G * F;
  float denominator = 4.0 * max(dot(normal, view_dir), 0.0) * ndl + 0.001;
  vec3 specular = numerator / denominator;

  vec3 k_s = F;
  vec3 k_d = vec3(1.0) - k_s;
  k_d *= 1.0 - metallic;

  float shadow = 1.0; // calculate_shadow(world_position, normal);
  vec3 irradiance = scene.light_color.rgb * ndl * shadow;

  color += (k_d * albedo / PI + specular) * irradiance;

  // --- Point Lights ---
  for (uint i = 0u; i < scene.point_light_count; ++i) {
    point_light light = point_lights.data[i];
    vec3 l_dir = light.position - world_position;
    float distance = max(length(l_dir), 0.001);
    l_dir /= distance;

    vec3 h = normalize(view_dir + l_dir);
    float ndl_p = max(dot(normal, l_dir), 0.0);
    float attenuation = clamp(1.0 - distance / light.radius, 0.0, 1.0);
    attenuation *= attenuation;

    vec3 radiance = light.color.rgb * light.color.a * attenuation;

    float Dp = distribution_ggx(normal, h, roughness);
    float Gp = geometry_smith(normal, view_dir, l_dir, roughness);
    vec3 Fp = fresnel_schlick(max(dot(h, view_dir), 0.0), f0);

    vec3 numerator_p = Dp * Gp * Fp;
    float denom_p = 4.0 * max(dot(normal, view_dir), 0.0) * ndl_p + 0.001;
    vec3 spec_p = numerator_p / denom_p;

    vec3 kd_p = vec3(1.0) - Fp;
    kd_p *= 1.0 - metallic;

    color += (kd_p * albedo / PI + spec_p) * radiance * ndl_p;
  }

  // --- Ambient ---
  vec3 ambient = vec3(0.03) * albedo * ao;
  vec3 final_color = ambient + color;

  // --- Output ---
  out_color = vec4(final_color, 1.0);
}
