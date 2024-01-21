#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_light_space_position;
layout(location = 4) in vec4 in_color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  mat4 light_space;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  float time;
} scene;

layout(binding = 2) uniform sampler2D image;
layout(binding = 3) uniform sampler2D shadow_map;

const material DEFAULT_MATERIAL = material(
  vec4(0.2, 0.2, 0.2, 1.0),   // Ambient color
  vec4(0.8, 0.8, 0.8, 1.0),   // Diffuse color
  vec4(1.0, 1.0, 1.0, 1.0),   // Specular color
  8.0                         // Shininess
);

const vec4 GROUND_COL = vec4(0.8, 1.0, 0.5, 1.0);
const vec4 SKY_COL = vec4(0.8, 0.8, 1.0, 1.0);
const float LOWEST_ALPHA = 0.4;

const int PCF_COUNT = 8;
const float RADIUS = 0.0004;
const float BIAS = 0.0005;
const float SAMPLE_WEIGHT = 1.0 / (PCF_COUNT * PCF_COUNT);
const vec2 OFFSET = vec2(0.5 - 0.5 * PCF_COUNT);

float calculate_shadow() {
  vec3 coordinates = in_light_space_position.xyz / in_light_space_position.w;

  if (coordinates.z > 1.0 || coordinates.z < -1.0) {
    return 0.0;
  }

	float total = 0.0;

	for(int x = 0; x < PCF_COUNT; x++) {
    for(int y = 0; y < PCF_COUNT; y++) {
      vec2 sample_position = vec2(x, y) + OFFSET;
      vec2 jitter = random_2d(gl_FragCoord.xy + vec2(13278 * x, 321 * y));

      float object_depth = texture(shadow_map, coordinates.xy + (sample_position + jitter) * RADIUS).r;

      float factor = (coordinates.z - BIAS) > object_depth ? 0.0 : 1.0;

      total += factor * SAMPLE_WEIGHT;
    }
	}

	return total;
}

void main(void) {
	// vec3 to_camera = normalize(scene.camera_position - in_normal);
	// vec3 to_light = normalize(-scene.light_direction);
	// vec3 reflected_view = reflect(-to_camera, in_normal);

  // float sun_diffuse = clamp(dot(in_normal, to_light), 0.0, 1.0);
	// float sky_diffuse = 0.7 + 0.3 * clamp(in_normal.y, 0.0, 1.0);
	// float fresnel = pow(1.0 - clamp(dot(to_camera, in_normal), 0.0, 1.0), 2);
	// float sun_specular = pow(clamp(dot(reflected_view, to_light), 0.0, 1.0), 2.0) * (0.8 + 5.0 * fresnel);
	// float rim_light = fresnel;
	// float sky_reflection = smoothstep(0.0, 0.6, reflected_view.y) * (0.4 + 0.6 * fresnel);

  float shadow = calculate_shadow();
  // float shadow = 0.0;

  vec3 view_direction = normalize(scene.camera_position - in_position);

  vec4 lighting = calculate_directional_light_blinn_phong(DEFAULT_MATERIAL, directional_light(scene.light_direction, scene.light_color), in_normal, view_direction);

  // out_color = vec4(light.rgb * 1.3 * shadow, 1.0);

	// vec4 lighting = vec4(0.0);

	// lighting += 1.4 * sky_diffuse * SKY_COL;
	// lighting += 1.3 * sky_diffuse * shadow * scene.light_color;

	// vec4 final_color = texture(image, in_uv) * lighting * in_color * shadow;
  vec4 final_color = texture(image, in_uv) * lighting * in_color;

	// final_color += 0.03 * sky_reflection * SKY_COL * sky_diffuse;
	// final_color += 0.04 * rim_light * SKY_COL;
	// final_color += 0.09 * sun_specular * shadow * scene.light_color;

  out_position = vec4(in_position, 1.0);
  out_normal = vec4(in_normal, 1.0);
  out_color = final_color;

	// out_color = vec4(pow(final_color, vec4(1.4545)));
  // out_color = vec4(shadow, shadow, shadow, 1.0);
  // out_color = vec4(texture(shadow_map, in_uv).rrr, 1.0);
}
