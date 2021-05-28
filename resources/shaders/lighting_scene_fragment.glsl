#version 330 core

struct material {
  sampler2D diffuse;
  vec3 specular;
  float shininess;
};

struct light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec2 vertex_uv;
in vec3 vertex_normal;
in vec3 vertex_fragment_position;

uniform vec3 uni_view_position;
uniform material uni_material;
uniform light uni_light;

out vec4 fragment_color;
  
void main() {
  // ambient lighting
  vec3 ambient = uni_light.ambient * vec3(texture(uni_material.diffuse, vertex_uv));

  // diffuse lighting
  vec3 normalized_normal = normalize(vertex_normal);
  vec3 normalized_light_direction = normalize(uni_light.position - vertex_fragment_position);
  float diffuse_value = max(dot(normalized_normal, normalized_light_direction), 0.0);
  vec3 diffuse = uni_light.diffuse * diffuse_value * vec3(texture(uni_material.diffuse, vertex_uv));

  // specular lighting
  vec3 view_direction = normalize(uni_view_position - vertex_fragment_position);
  vec3 reflection_direction = reflect(-normalized_light_direction, normalized_normal);  
  float specular_value = pow(max(dot(view_direction, reflection_direction), 0.0), uni_material.shininess);
  vec3 specular = uni_light.specular * (specular_value * uni_material.specular);

  vec3 result = ambient + diffuse + specular;

  fragment_color = vec4(result, 1.0);
  // fragment_color = texture(uni_texture, vertex_uv) * vec4(result, 1.0);
}
