#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"

layout(location = 0) in vec4 in_color;

layout(location = 0) out vec4 out_color;

void main(void) {
  out_color = in_color;
}
