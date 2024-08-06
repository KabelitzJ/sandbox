#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/shadow.glsl"
#include "../common/depth.glsl"
#include "../common/constants.glsl"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(binding = 1, std430) readonly buffer buffer_point_lights {
  point_light data[];
} point_lights;

layout(binding = 2, input_attachment_index = 0) uniform subpassInput position_image; 
layout(binding = 3, input_attachment_index = 1) uniform subpassInput normal_image;
layout(binding = 4, input_attachment_index = 2) uniform subpassInput albedo_image;
layout(binding = 5, input_attachment_index = 3) uniform subpassInput material_image;

layout(binding = 6) uniform sampler2D shadow_map_image;

const material DEFAULT_MATERIAL = material(
  vec4(1.0, 1.0, 1.0, 1.0),     // Ambient color
  vec4(0.04, 0.04, 0.04, 1.0),  // Specular color
  32.0,                          // Metallic
  0.5,                          // Roughness
  0.5                           // Ambient occlusion
);

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

float distribution_ggx(vec3 N, vec3 H, float roughness) {
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float geometry_schlick_ggx(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float nom   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = geometry_schlick_ggx(NdotV, roughness);
  float ggx1 = geometry_schlick_ggx(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

#define PBR 1

void main() {
  vec3 world_position = subpassLoad(position_image).xyz;
  vec3 normal = normalize(subpassLoad(normal_image).xyz);
  vec4 albedo = subpassLoad(albedo_image);
  vec3 material = subpassLoad(material_image).rgb;

  float metallic = material.r;
  float roughness = material.g;
  float ambient_occlusion = material.b;

#if PBR

  vec3 N = normalize(normal);
  vec3 V = normalize(scene.camera_position - world_position);

  vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);

  vec3 Lo = vec3(0.0);

  for (uint i = 0; i < min(MAX_POINT_LIGHTS, scene.point_light_count); i++) {
    point_light light = point_lights.data[i];

    vec3 L = normalize(light.position - world_position);
    vec3 H = normalize(V + L);

    float distance = length(light.position - world_position);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color.rgb * attenuation;

    float NDF = distribution_ggx(N, H, roughness);
    float G = geometry_smith(N, V, L, roughness);
    vec3 F = fresnel_schlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
  }

  vec3 ambient = vec3(0.03) * albedo.rgb * ambient_occlusion;

  vec3 color = ambient + Lo;

  color = color / (color + vec3(1.0));

  color = pow(color, vec3(1.0 / 2.2));

  out_color = vec4(color, 1.0);

#else

  vec4 total_light = vec4(0.0);

  directional_light light = directional_light(scene.light_direction, scene.light_color);

  light_result result = calculate_directional_light(light, world_position, normal, scene.camera_position, DEFAULT_MATERIAL);

  total_light += (result.ambient + result.diffuse + result.specular) * albedo;

  for (uint i = 0; i < min(scene.point_light_count, MAX_POINT_LIGHTS); i++) {
    point_light light = point_lights.data[i];

    light_result result = calculate_point_light(light, world_position, normal, scene.camera_position, DEFAULT_MATERIAL);

    total_light += (result.ambient + result.diffuse + result.specular) * albedo;
  }

  vec4 light_space_position = DEPTH_BIAS * scene.light_space * vec4(world_position, 1.0);

  float shadow = calculate_shadow_random_jitter(shadow_map_image, light_space_position, normal, scene.light_direction);

  out_color = total_light * shadow;
#endif
}
