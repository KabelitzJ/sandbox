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

layout(binding = 1) buffer buffer_point_lights {
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

const vec4 GROUND_COLOR = vec4(0.8, 1.0, 0.5, 1.0);
const vec4 SKY_COLOR = vec4(0.8, 0.8, 1.0, 1.0);
const float LOWEST_ALPHA = 0.4;
const float BRIGHTNESS_EFFECT = 0.5;

void main() {
  vec3 world_position = subpassLoad(position_image).xyz;
  vec3 normal = normalize(subpassLoad(normal_image).xyz);
  vec4 albedo = subpassLoad(albedo_image);
  vec3 material = subpassLoad(material_image).rgb;

  float metallic = material.r;
  float roughness = material.g;
  float ambient_occlusion = material.b;

  vec3 to_camera = normalize(scene.camera_position - world_position);
  vec3 to_light = normalize(-scene.light_direction);
  vec3 reflected = reflect(to_camera, normal);

  float sun_diffuse = max(dot(normal, to_light), 0.0);
	float sky_diffuse = 0.7 + 0.3 * clamp(normal.y, 0.0, 1.0);
	float fresnel = pow(1.0 - max(dot(to_camera, normal), 0.0), 2);
	float sun_specular = pow(max(dot(reflected, -scene.light_direction), 0.0), 2.0) * (0.8 + 5.0 * fresnel);
	float rim_light = fresnel;
	float sky_reflection = smoothstep(0.0, 0.6, reflected.y) * (0.4 + 0.6 * fresnel);

  vec4 light_space_position = DEPTH_BIAS * scene.light_space * vec4(world_position, 1.0);

  float shadow = calculate_shadow_random_jitter(shadow_map_image, light_space_position, normal, to_light);

  vec4 lighting = vec4(0.0);

	lighting += 1.4 * sky_diffuse  * SKY_COLOR;
	lighting += 1.3 * sun_diffuse * shadow * scene.light_color;

	vec4 final_color = albedo * lighting;
	final_color += 0.03 * sky_reflection * SKY_COLOR * sky_diffuse;
	final_color += 0.04 * rim_light * SKY_COLOR;
	final_color += 0.09 * sun_specular * shadow * scene.light_color;

	out_color = final_color;
}
