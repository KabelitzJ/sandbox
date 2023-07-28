#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(binding = 1) uniform sampler2D image;

layout(location = 0) out vec4 out_color;

// const vec4 ambient_color  = vec4(0.87, 0.21, 0.12, 1.0);
// const vec4 diffuse_color = vec4(0.87, 0.21, 0.12, 1.0);
const vec4 ambient_color  = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 diffuse_color  = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 specular_color = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 shininess = vec4(32.0, 0.0, 0.0, 0.0);

const vec4 camera_position = vec4(2.0, 2.0, 1.0, 0.0);
const vec4 light_position = vec4(-1.0, 3.0, 1.0, 0.0);
const vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0);

vec4 phong_shading(vec3 light_direction, float intensity) {
  // Calculate the ambient color
  vec4 ambient = light_color * ambient_color;

  // Calculate the diffuse color
  vec4 diffuse = light_color * diffuse_color * intensity;

  // Calculate the specular color
  vec3 camera_direction = normalize(vec3(camera_position) - in_position);
  vec3 reflection_direction = reflect(-light_direction, in_normal);
  float specular_intensity = pow(max(dot(camera_direction, reflection_direction), 0.0), shininess.x);
  vec4 specular = light_color * specular_color * specular_intensity;

  // Calculate the final color
  return (ambient + diffuse + specular);
}

vec4 cel_shading(vec3 light_direction, float intensity) {
  const int CEL_LEVELS = 4;
  const vec4 SHADOW_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

  // Calculate the index of the shade based on the intensity
  float shade_index = floor(intensity * float(CEL_LEVELS));
  
  // Calculate the color based on the shade index
  return mix(SHADOW_COLOR, ambient_color, shade_index / float(CEL_LEVELS - 1));
}

void main() {
  vec3 light_direction = normalize(vec3(light_position) - in_position);
  float intensity = max(dot(in_normal, light_direction), 0.0);
 
  vec4 phong_shading = phong_shading(light_direction, intensity);
  vec4 cel_shading = cel_shading(light_direction, intensity);

  float mix_factor = 0.0;

  out_color = texture(image, in_uv) * mix(phong_shading, cel_shading, mix_factor);
}
