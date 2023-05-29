#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform push_constant {
  vec4 light_color;
  vec4 ambient_color;
  vec4 camera_position;
  vec4 light_position;
} uniform_push_constant;

/**
 * Calculates the phong shading for the given light direction and intensity.
 *
 * @param light_direction The direction of the light.
 * @param intensity The intensity of the light.
 *
 * @return The phong shading.
 */
vec4 phong_shading(vec3 light_direction, float intensity) {
  // Calculate the ambient color
  vec4 ambient = uniform_push_constant.ambient_color;

  // Calculate the diffuse color
  vec4 diffuse = uniform_push_constant.light_color * intensity;

  // Calculate the specular color
  vec3 camera_direction = normalize(vec3(uniform_push_constant.camera_position) - in_position);
  vec3 reflection_direction = reflect(-light_direction, in_normal);
  float specular_intensity = pow(max(dot(camera_direction, reflection_direction), 0.0), 32);
  vec4 specular = uniform_push_constant.light_color * specular_intensity;

  return (ambient + diffuse + specular) * in_color;
}

vec4 cel_shading(vec3 light_direction, float intensity) {
  const int CEL_LEVELS = 4;
  const vec4 SHADOW_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

  // Calculate the index of the shade based on the intensity
  float shade_index = floor(intensity * float(CEL_LEVELS));
  
  // Calculate the color based on the shade index
  vec4 cel_color = mix(SHADOW_COLOR, in_color, shade_index / float(CEL_LEVELS - 1));
  
  // Output the final color
  return cel_color;
}

void main() {
  vec3 light_direction = normalize(vec3(uniform_push_constant.light_position) - in_position);
  float intensity = max(dot(in_normal, light_direction), 0.0);
 
  vec4 phong_shading = phong_shading(light_direction, intensity);
  vec4 cel_shading = cel_shading(light_direction, intensity);

  float mix_factor = 0.25;

  out_color = mix(phong_shading, cel_shading, mix_factor);
}
