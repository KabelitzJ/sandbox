#version 450

layout(location = 0) in vec2 in_uv;

layout(binding = 1) uniform sampler2D image;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform push_data {
	vec2 direction;
  uint type;
} data;

const uint BLUR_TYPE_GAUSSIAN_5 = 0;
const uint BLUR_TYPE_GAUSSIAN_9 = 1;
const uint BLUR_TYPE_GAUSSIAN_13 = 2;

vec4 gaussian_blur_5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);

  vec2 off1 = vec2(1.3333333333333333) * direction;

  color += texture(image, uv) * 0.29411764705882354;
  color += texture(image, uv + (off1 / resolution)) * 0.35294117647058826;
  color += texture(image, uv - (off1 / resolution)) * 0.35294117647058826;

  return color; 
}

vec4 gaussian_blur_9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);

  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;

  color += texture(image, uv) * 0.2270270270;
  color += texture(image, uv + (off1 / resolution)) * 0.3162162162;
  color += texture(image, uv - (off1 / resolution)) * 0.3162162162;
  color += texture(image, uv + (off2 / resolution)) * 0.0702702703;
  color += texture(image, uv - (off2 / resolution)) * 0.0702702703;

  return color;
}

vec4 gaussian_blur_13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);

  vec2 off1 = vec2(1.411764705882353) * direction;
  vec2 off2 = vec2(3.2941176470588234) * direction;
  vec2 off3 = vec2(5.176470588235294) * direction;

  color += texture(image, uv) * 0.1964825501511404;
  color += texture(image, uv + (off1 / resolution)) * 0.2969069646728344;
  color += texture(image, uv - (off1 / resolution)) * 0.2969069646728344;
  color += texture(image, uv + (off2 / resolution)) * 0.09447039785044732;
  color += texture(image, uv - (off2 / resolution)) * 0.09447039785044732;
  color += texture(image, uv + (off3 / resolution)) * 0.010381362401148057;
  color += texture(image, uv - (off3 / resolution)) * 0.010381362401148057;

  return color;
}

void main() {
  vec2 resolution = vec2(textureSize(image, 0));

  vec4 color = vec4(0.0);

  if (data.type == BLUR_TYPE_GAUSSIAN_5) {
    color = gaussian_blur_5(image, in_uv, resolution, data.direction);
  } else if (data.type == BLUR_TYPE_GAUSSIAN_9) {
    color = gaussian_blur_9(image, in_uv, resolution, data.direction);
  } else if (data.type == BLUR_TYPE_GAUSSIAN_13) {
    color = gaussian_blur_13(image, in_uv, resolution, data.direction);
  }

  out_color = color;
}
