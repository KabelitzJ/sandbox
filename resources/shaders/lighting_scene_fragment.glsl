#version 330 core

in vec2 vertex_uv;
in vec3 vertex_normal;
in vec3 vertex_fragment_position;

uniform vec3 uni_light_position;
uniform vec3 uni_light_color;
uniform vec3 uni_object_color;
uniform sampler2D uni_texture;

out vec4 fragment_color;
  
void main() {
  // ambient lighting
  float ambient_strength = 0.1;
  vec3 ambient = uni_light_color * ambient_strength;

  // diffuse lighting
  vec3 normalized_normal = normalize(vertex_normal);
  vec3 normalized_light_direction = normalize(uni_light_position - vertex_fragment_position);

  float difference = max(dot(normalized_normal, normalized_light_direction), 0.0);
  vec3 diffuse = difference * uni_light_color;

  fragment_color = texture(uni_texture, vertex_uv) * vec4(uni_object_color * (ambient + difference), 1.0);
}
