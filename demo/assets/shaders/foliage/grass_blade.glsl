#ifndef FOLIAGE_GRASS_BLADE_GLSL_
#define FOLIAGE_GRASS_BLADE_GLSL_

struct grass_blade {
	vec4 v0;  // v0.w -- orientation
  vec4 v1;  // v1.w -- height
  vec4 v2;  // v2.w -- width
  vec4 up;  // up.w -- stiffness
}; // struct grass_blade

layout(buffer_reference, std430) buffer grass_buffer_reference {
	grass_blade data[];
}; // buffer grass_buffer_reference

layout(buffer_reference, std430) readonly buffer readonly_grass_buffer_reference {
	grass_blade data[];
}; // buffer grass_buffer_reference

layout(buffer_reference, std430) writeonly buffer writeonly_grass_buffer_reference {
	grass_blade data[];
}; // buffer grass_buffer_reference

#endif // FOLIAGE_GRASS_BLADE_GLSL_
