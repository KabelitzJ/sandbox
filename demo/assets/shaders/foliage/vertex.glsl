#version 460 core

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : enable

#include <foliage/grass_blade.glsl>

// Outputs are passed to tessellation shaders
layout(location = 0) out vec4 out_v0;
layout(location = 1) out vec4 out_v1;
layout(location = 2) out vec4 out_v2;
layout(location = 3) out vec4 out_up;

layout(push_constant) uniform push_constants {
	grass_buffer_reference blades;
	mat4 model;
	mat4 view_projection;
	vec4 green_bottom;
	vec4 green_top;
	float global_time;
};

void main() {
  out_v0 = vec4((model * vec4(v0.xyz, 1.f)).xyz, v0.w);
  out_v1 = vec4((model * vec4(v1.xyz, 1.f)).xyz, v1.w);
  out_v2 = vec4((model * vec4(v2.xyz, 1.f)).xyz, v2.w);
  out_up = vec4((model * vec4(up.xyz, 0.f)).xyz, up.w);
	
	gl_Position = model * vec4(v0.xyz, 1.f);
}
