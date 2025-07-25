#version 460 core

#include <libsbx/common/math.glsl>

#include <grid/config.glsl>

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec2 in_camera_position;

layout(location = 0) out vec4 out_color;

vec4 grid_color(vec2 uv, vec2 camera_position) {
  vec2 dudv = vec2(length(vec2(dFdx(uv.x), dFdy(uv.x))), length(vec2(dFdx(uv.y), dFdy(uv.y))));

  float lod_level = max(0.0, log10((length(dudv) * grid_min_pixels_between_cells) / grid_cell_size) + 1.0);
  float lod_fade = fract(lod_level);

  float lod0 = grid_cell_size * pow(10.0, floor(lod_level));
  float lod1 = lod0 * 10.0;
  float lod2 = lod1 * 10.0;

  dudv *= 4.0;
  uv += dudv * 0.5;

  float lod0a = max2(vec2(1.0) - abs(satv(mod(uv, lod0) / dudv) * 2.0 - vec2(1.0)));
  float lod1a = max2(vec2(1.0) - abs(satv(mod(uv, lod1) / dudv) * 2.0 - vec2(1.0)));
  float lod2a = max2(vec2(1.0) - abs(satv(mod(uv, lod2) / dudv) * 2.0 - vec2(1.0)));

  vec4 color = lod2a > 0.0 ? grid_color_thick : lod1a > 0.0 ? mix(grid_color_thick, grid_color_thin, lod_fade) : grid_color_thin;

  uv -= camera_position;
  float opacity_falloff = (1.0 - satf(length(uv) / grid_size));

  color.a *= lod2a > 0.0 ? lod2a : lod1a > 0.0 ? lod1a : (lod0a * (1.0 - lod_fade));
  color.a *= opacity_falloff;

  return color;
}

void main() {
  out_color = grid_color(in_uv, in_camera_position);
}
