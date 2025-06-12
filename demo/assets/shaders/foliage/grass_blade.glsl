#ifndef FOLIAGE_GRASS_BLADE_GLSL_
#define FOLIAGE_GRASS_BLADE_GLSL_

struct grass_blade {
	vec4 position_bend;         // xyz = position, w = bend amount
	vec4 size_animation_pitch;  // x = width, y = height, z = animation, w = pitch
}; // struct grass_blade

layout(buffer_reference, std430) readonly buffer grass_buffer_reference {
	grass_blade data[];
}; // buffer grass_buffer_reference

#endif // FOLIAGE_GRASS_BLADE_GLSL_
