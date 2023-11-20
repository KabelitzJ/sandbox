#version 450

#include "../common/lighting.glsl"

struct material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_light_space_position;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(binding = 1) buffer buffer_point_lights {
  point_light data[];
} point_lights;

layout(binding = 2) uniform sampler2D image;
layout(binding = 3) uniform sampler2D shadow_map;

layout(push_constant) uniform uniform_object {
  mat4 model;
  mat4 normal;
  vec4 tint;
  int uses_lighting;
} object;

const material default_material = material(
  vec4(1.0, 1.0, 1.0, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  vec4(0.5, 0.5, 0.5, 1.0),
  32.0
);

const vec3 light_position = vec3(4.0, 6.0, 4.0);

float calculate_shadow(vec4 light_space_position, vec3 light_dir) {
  // float shadow = 0.0;

  // vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

  float bias = max(0.005 * (1.0 - dot(in_normal, light_dir)), 0.0005);

  // for (int x = -1; x <= 1; ++x) {
  //   for (int y = -1; y <= 1; ++y) {
  //     float pcf_depth = texture(shadow_map, light_space_position.xy + vec2(x, y) * texel_size).r;
  //     shadow += (light_space_position.z - bias) > pcf_depth ? 1.0 : 0.0;
  //   }
  // }

  // return shadow / 9.0;

  vec3 proj_coords = light_space_position.xyz / light_space_position.w;

  float current_depth = proj_coords.z;
  float closest_depth = texture(shadow_map, proj_coords.xy).r;

  return current_depth - bias > closest_depth ? 1.0 : 0.0;
}

void main() {
  vec4 color = texture(image, in_uv);

  vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);
  // ambient
  vec4 ambient = 0.15 * light_color;
  // diffuse
  vec3 light_dir = normalize(light_position - in_position);
  float diff = max(dot(light_dir, in_normal), 0.0);
  vec4 diffuse = diff * light_color;
  // specular
  vec3 view_dir = normalize(scene.camera_position - in_position);
  float spec = 0.0;
  vec3 halfway_dir = normalize(light_dir + view_dir);  
  spec = pow(max(dot(in_normal, halfway_dir), 0.0), 64.0);
  vec4 specular = spec * light_color;    
  // calculate shadow
  float shadow = calculate_shadow(in_light_space_position, light_dir);  

  // out_color = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
  // out_color = (ambient + diffuse + specular) * color;

  out_color = vec4(shadow, shadow, shadow, 1.0);
  // out_color = vec4(texture(shadow_map, in_uv).rrr, 1.0);
}
