#version 450

layout(location = 0) in vec4 in_color;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform push_constant {
  vec4 color;
} uniform_push_constant;

void main() {
  out_color = in_color * 0.5 + uniform_push_constant.color * 0.5;
}
