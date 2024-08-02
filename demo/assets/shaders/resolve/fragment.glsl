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
} scene;

layout(binding = 1, input_attachment_index = 0) uniform subpassInput position_image; 
layout(binding = 2, input_attachment_index = 1) uniform subpassInput normal_image;
layout(binding = 3, input_attachment_index = 2) uniform subpassInput albedo_image;
layout(binding = 4, input_attachment_index = 3) uniform subpassInput material_image;

layout(binding = 5) uniform sampler2D shadow_map_image;

const material DEFAULT_MATERIAL = material(
  vec4(1.0, 1.0, 1.0, 1.0),   // Ambient color
  vec4(1.0, 1.0, 1.0, 1.0),   // Diffuse color
  vec4(0.5, 0.5, 0.5, 1.0),   // Specular color
  32.0                        // Shininess
);

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

const int POINT_COUNT = 4;

const point_light[] LIGHTS = point_light[POINT_COUNT](
  point_light(vec3(15.0, 15.0, 3.0), vec4(1.0, 0.0, 0.0, 1.0), 10.0),
  point_light(vec3(-15.0, 15.0, 3.0), vec4(0.0, 1.0, 0.0, 1.0), 10.0),
  point_light(vec3(15.0, -15.0, 3.0), vec4(0.0, 0.0, 1.0, 1.0), 10.0),
  point_light(vec3(-15.0, -15.0, 3.0), vec4(1.0, 1.0, 1.0, 1.0), 10.0)
  // point_light(vec3(5.0, 5.0, 3.0), vec4(1.0, 1.0, 1.0, 1.0), 10.0),
  // point_light(vec3(-5.0, 5.0, 3.0), vec4(1.0, 1.0, 1.0, 1.0), 10.0),
  // point_light(vec3(5.0, -5.0, 3.0), vec4(1.0, 1.0, 1.0, 1.0), 10.0),
  // point_light(vec3(-5.0, -5.0, 3.0), vec4(1.0, 1.0, 1.0, 1.0), 10.0),
  // point_light(vec3(0.0, 0.0, 50.0), vec4(1.0, 1.0, 1.0, 1.0), 10.0)
);

vec4 cel_shading(material material, directional_light light, vec3 normal) {
  const int CEL_LEVELS = 5;
  const vec4 SHADOW_COLOR = vec4(0.1, 0.1, 0.1, 1.0);

  float intensity = max(dot(normal, light.direction), 0.0);

  // Calculate the index of the shade based on the intensity
  float shade_index = floor(intensity * float(CEL_LEVELS));
  
  // Calculate the color based on the shade index
  // return mix(material.ambient, SHADOW_COLOR, shade_index / float(CEL_LEVELS - 1));
  return material.ambient * (1.0 - shade_index / float(CEL_LEVELS - 1));
}

vec3 fresnel_schlick(float cos_theta, vec3 f0) {
  return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float distribution_ggx(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float num   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return num / denom;
}

float geometry_schlick_ggx(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float num   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return num / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = geometry_schlick_ggx(NdotV, roughness);
  float ggx1 = geometry_schlick_ggx(NdotL, roughness);

  return ggx1 * ggx2;
}

void main() {
  vec3 world_position = subpassLoad(position_image).xyz;
  vec3 normal = subpassLoad(normal_image).xyz;
  vec4 albedo = subpassLoad(albedo_image);
  vec3 material = subpassLoad(material_image).rgb;

  float metallic = material.r;
  float roughness = material.g;
  float ambient_occlusion = material.b;

  vec3 N = normalize(normal);
  vec3 V = normalize(scene.camera_position - world_position);

  vec3 F0 = vec3(0.77, 0.78, 0.78); 
  F0 = mix(F0, albedo.rgb, metallic);
            
  // reflectance equation
  vec3 Lo = vec3(0.0);
  for(int i = 0; i < POINT_COUNT; ++i) {
    // calculate per-light radiance
    point_light light = LIGHTS[i];

    vec3 L = normalize(light.position - world_position);
    vec3 H = normalize(V + L);
    float distance    = length(light.position - world_position);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = light.color.rgb * attenuation;        
    
    // cook-torrance brdf
    float NDF = distribution_ggx(N, H, roughness);        
    float G   = geometry_smith(N, V, L, roughness);      
    vec3 F    = fresnel_schlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL; 
  }   

  vec3 ambient = vec3(0.03) * albedo.rgb * ambient_occlusion;
  vec3 color = ambient + Lo;

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));  
  
  out_color = vec4(color, 1.0);

  // vec4 light_space_position = (DEPTH_BIAS * scene.light_space) * vec4(position, 1.0);

  // vec3 view_direction = normalize(scene.camera_position - position);
  // 
  // directional_light light = directional_light(scene.light_direction, scene.light_color);

  // light_result light_result = blinn_phong_shading(DEFAULT_MATERIAL, light, normal, view_direction);

  // float shadow_factor = calculate_shadow_pcf(shadow_map_image, light_space_position, normal, light.direction);
  // float shadow_factor = calculate_shadow_random_jitter(shadow_map_image, light_space_position, normal, light.direction);

  // vec4 lighting = light_result.ambient + (light_result.diffuse + light_result.specular) * shadow_factor;

  // vec4 cel_color = cel_shading(DEFAULT_MATERIAL, light, normal);

  // out_color = albedo * mix(cel_color, lighting, 1.0);
  // out_color = vec4(vec3(texture(shadow_map_image, in_uv).r), 1.0);
  // out_color = vec4(vec3(texture(depth_image, in_uv).r), 1.0);
  // out_color = vec4(normal, 1.0);
}
