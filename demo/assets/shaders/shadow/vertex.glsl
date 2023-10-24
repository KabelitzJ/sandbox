#version 450

layout(location = 0) in vec3 in_position;

layout(push_constant) uniform uniform_object {
  mat4 model;
  mat4 view;
  mat4 projection;
} object;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = object.projection * object.view * object.model * vec4(in_position, 1.0);
}
