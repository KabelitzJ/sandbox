#version 450

layout(location = 0) out vec4 out_color;

void main() {
  float depth = gl_FragCoord.z;

  float dx = dFdx(depth);
  float dy = dFdy(depth);

  float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

  out_color = vec4(depth, moment2, 0.0, 0.0);
}
