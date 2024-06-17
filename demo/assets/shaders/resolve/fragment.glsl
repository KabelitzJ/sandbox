#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/shadow.glsl"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
} scene;

layout(binding = 1) uniform sampler2D position_image; 
layout(binding = 2) uniform sampler2D normal_image;
layout(binding = 3) uniform sampler2D albedo_image;
layout(binding = 4) uniform sampler2D shadow_map_image;

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

float pcf(vec4 light_space_position, vec3 normal, vec3 light_direction) {
  // perform perspective divide
  vec3 projected_coords = light_space_position.xyz / light_space_position.w;

  if(projected_coords.z > 1.0) {
    return 0.0;
  }
  
  // float bias = max(0.00025 * (1.0 - dot(normal, light_direction)), 0.000025);
  float bias = 0.000025;

  // check whether current frag pos is in shadow
  
  float shadow = 0.0;
  vec2 size = 1.0 / textureSize(shadow_map_image, 0);

  int pcf_count = 1;
  int pfc_iterations = 0;

  for(int x = -pcf_count; x <= pcf_count; ++x) {
    for(int y = -pcf_count; y <= pcf_count; ++y) {
      float pcf_depth = texture(shadow_map_image, projected_coords.xy + vec2(x, y) * size).r; 
      shadow += (projected_coords.z - bias) > pcf_depth ? 1.0 : 0.0;
      pfc_iterations++;    
    }    
  }

  return shadow / pfc_iterations;
}

void main() {
  vec3 position = texture(position_image, in_uv).xyz;
  vec4 light_space_position = (DEPTH_BIAS * scene.light_space) * vec4(position, 1.0);
  vec3 normal = texture(normal_image, in_uv).xyz;
  vec4 albedo = texture(albedo_image, in_uv);

  vec3 view_direction = normalize(scene.camera_position - position);
  
  directional_light light = directional_light(scene.light_direction, scene.light_color);

  blinn_phong_result lighting_result = calculate_directional_light_blinn_phong(DEFAULT_MATERIAL, light, normal, view_direction);

  float shadow_factor = pcf(light_space_position, normal, light.direction);

  out_color = albedo * (lighting_result.ambient + (1.0 - shadow_factor) * (lighting_result.diffuse + lighting_result.specular));

  // out_color = vec4(vec3(texture(shadow_map_image, in_uv).r), 1.0);
}
