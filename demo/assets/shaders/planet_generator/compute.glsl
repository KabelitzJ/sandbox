#version 450 core

layout(local_size_x = 256) in;

struct particle {
  vec3 position;
  vec3 velocity;
  vec4 color;
}; // struct particle

layout(set = 0, binding = 0) uniform uniform_config {
  float delta_time;
} config;


layout(binding = 1, std430) readonly buffer buffer_in_particles {
  particle particles[];
} in_particles;

layout(binding = 2, std430) buffer buffer_out_particles {
  particle particles[]
} out_particles;

void main() {
  uint id = gl_GlobalInvocationID.x;

  buffer_out_particles.particles[id] = buffer_in_particles.particles[id].position + buffer_in_particles.particles[id].velocity * config.delta_time;
}
