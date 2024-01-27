#version 450

layout(location = 0) out vec2 out_color;

void main() {
  float depth = gl_FragCoord.z;

  float dx = dFdx(depth);
  float dy = dFdy(depth);

  float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

  out_color = vec2(depth, moment2);
}
