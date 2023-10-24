#version 450

layout(location = 0) in vec3 in_position;

in vec4 gl_FragCoord;


layout(location = 0) out vec4 out_color;

out float gl_FragDepth;



void main() {
  gl_FragDepth = gl_FragCoord.z;

  out_color = vec4(0.0, 0.0, 0.0, 1.0);
}
