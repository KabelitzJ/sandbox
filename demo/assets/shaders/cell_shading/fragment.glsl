#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec4 camera_position;
} scene;

layout(binding = 1) uniform sampler2D image;

struct material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct light {
  vec3 position;
  vec3 color;
};

const material default_material = material(
  vec3(1.0, 1.0, 1.0),
  vec3(1.0, 1.0, 1.0),
  vec3(0.5, 0.5, 0.5),
  32.0
);

const light default_light = light(
  vec3(3.0, 3.0, 3.0),
  vec3(1.0, 0.97, 0.84)
);

vec3 phong_shading(vec3 light_direction, float intensity) {
  // Calculate the ambient color
  vec3 ambient = (default_light.color * 0.1) * default_material.ambient;

  // Calculate the diffuse color
  vec3 diffuse = (default_light.color * 0.5) * (default_material.diffuse * intensity);

  // Calculate the specular color
  vec3 camera_direction = normalize(vec3(scene.camera_position) - in_position);
  vec3 reflection_direction = reflect(-light_direction, in_normal);
  float specular_intensity = pow(max(dot(camera_direction, reflection_direction), 0.0), default_material.shininess);
  vec3 specular = (default_material.specular * specular_intensity);

  // Calculate the final color
  return (ambient + diffuse + specular);
}

vec3 cel_shading(float intensity) {
  const int CEL_LEVELS = 4;
  const vec3 SHADOW_COLOR = vec3(0.0, 0.0, 0.0);

  // Calculate the index of the shade based on the intensity
  float shade_index = floor(intensity * float(CEL_LEVELS));
  
  // Calculate the color based on the shade index
  return mix(SHADOW_COLOR, default_material.ambient, shade_index / float(CEL_LEVELS - 1));
}

void main() {
  vec3 light_direction = normalize(vec3(default_light.position) - in_position);
  float intensity = max(dot(in_normal, light_direction), 0.0);
 
  vec3 phong_shading = phong_shading(light_direction, intensity);
  vec3 cel_shading = cel_shading(intensity);

  float mix_factor = 0.25;

  out_color = texture(image, in_uv) * vec4(mix(phong_shading, cel_shading, mix_factor), 1.0);
}