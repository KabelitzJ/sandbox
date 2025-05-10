#version 450 core

struct glyph {
  vec2 offset;
  vec2 size;
  vec2 uv_offset;
  vec2 uv_size;
};

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 out_uv;

layout(binding = 0) uniform uniform_scene {
  mat4 projection;
} scene;

layout(binding = 1, std430) readonly buffer buffer_glyphs {
  glyph data[];
} glyphs;

vec2 calculate_uv(glyph glyph) {
  return glyph.uv_offset + in_uv * glyph.uv_size;
}

vec2 calculate_position(glyph glyph) {
  return glyph.offset + in_position * glyph.size;
}

void main() {
  glyph glyph = glyphs.data[gl_InstanceIndex];

  out_uv = calculate_uv(glyph);
  vec2 position = calculate_position(glyph); 

  gl_Position = scene.projection * vec4(position, 0.0, 1.0);
}
