#version 450

struct material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

struct point_light {
  vec4 color;
  vec3 position;
  float radius;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  int light_count;
} scene;

layout(binding = 1) buffer buffer_lights {
  point_light array[];
} lights;

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

const point_light default_light = point_light(
  vec4(1.0, 0.97, 0.84, 1.0),
  vec3(5.0, 5.0, 5.0),
  10.0
);

const float mix_factor = 0.25;

vec4 phong_shading(vec3 light_direction, float intensity) {
  // Calculate the ambient color
  vec4 ambient = (default_light.color * 0.1) * default_material.ambient;

  // Calculate the diffuse color
  vec4 diffuse = (default_light.color * 0.5) * (default_material.diffuse * intensity);

  // Calculate the specular color
  vec3 camera_direction = normalize(vec3(scene.camera_position) - in_position);
  vec3 halfway_direction = normalize(light_direction + camera_direction);
  float specular_intensity = pow(max(dot(in_normal, halfway_direction), 0.0), default_material.shininess);
  vec4 specular = (default_material.specular * specular_intensity);

  // Calculate the final color
  return (ambient + diffuse + specular);
}

vec4 cel_shading(float intensity) {
  const int CEL_LEVELS = 3;
  const vec4 SHADOW_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

  // Calculate the index of the shade based on the intensity
  float shade_index = floor(intensity * float(CEL_LEVELS));
  
  // Calculate the color based on the shade index
  return mix(SHADOW_COLOR, default_material.ambient, shade_index / float(CEL_LEVELS - 1));
}

vec4 shading(vec3 light_direction, float intensity) {
  vec4 phong_shading = phong_shading(light_direction, intensity);
  vec4 cel_shading = cel_shading(intensity);

  return mix(phong_shading, cel_shading, mix_factor);
}

vec4 sum_shading() {
  vec4 shaded_color = vec4(0.0, 0.0, 0.0, 1.0);

  for (int i = 0; i < scene.light_count; ++i) {
    point_light current_light = lights.array[i];

    vec3 light_direction = normalize(current_light.position - in_position);

    float light_distance = length(light_direction);

    if (light_distance > current_light.radius) {
      continue;
    }

    float attenuation = 1.0 - (light_distance / current_light.radius);

    float intensity = max(dot(in_normal, light_direction), 0.0);

    shaded_color += shading(light_direction, intensity) * current_light.color * attenuation;
  }

  return shaded_color;
}

void main() {
  vec4 shaded_color = vec4(1.0, 1.0, 1.0, 1.0);

  if (object.uses_lighting == 1) {
    shaded_color = sum_shading();
  }

  out_color = texture(image, in_uv) * object.tint * shaded_color;
}
