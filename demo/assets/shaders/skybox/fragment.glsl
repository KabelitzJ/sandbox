#version 450

layout(binding = 1) uniform uniform_object {
  mat4 model;
  vec4 tint;
} object;

layout(binding = 2) uniform samplerCube skybox;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec4 out_color;

const float FOG_DISTANCE = 400.0;
const float FOG_DENSITY = 0.000;
const vec4 FOG_COLOR = vec4(0.5, 0.5, 0.5, 1.0);

void main() {
  vec4 cube_map_color = texture(skybox, normalize(in_position)) * object.tint;

  float fog_factor = clamp(1.0 - exp(-FOG_DISTANCE * FOG_DENSITY), 0.0, 1.0);

  out_color = mix(cube_map_color, FOG_COLOR, fog_factor);
}
