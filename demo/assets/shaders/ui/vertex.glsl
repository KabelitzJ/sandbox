#version 450

struct glyph {
  vec2 offset;
  vec2 uv;
};

layout(location = 0) in vec2 in_position;

layout(location = 0) out vec2 out_uv;

layout(binding = 0) uniform uniform_scene {
  mat4 projection;
} scene;

layout(binding = 1) buffer buffer_glyphs {
  glyph data[];
} glyphs;

void main() {
  glyph glyph = glyphs.data[gl_InstanceIndex];

  out_uv = in_uv;

  gl_Position = scene.projection * transforms.data[gl_InstanceIndex] * vec4(in_position, 0.0, 1.0);
}
