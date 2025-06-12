#version 460 core

#extension GL_EXT_buffer_reference : enable

#include <libsbx/common/vk.glsl>
#include <foliage/grass_blade.glsl>

#define WORKGROUP_SIZE 32

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(buffer_reference, std430) readonly buffer grass_input_reference {
  grass_blade data[];
};

layout(buffer_reference, std430) writeonly buffer grass_output_reference {
  grass_blade data[];
};

layout(buffer_reference, std430) buffer draw_command_reference {
  uint vertex_count;
  uint instance_count;
  uint first_vertex;
  uint first_instance;
};

layout(push_constant) uniform push_constants {
  grass_buffer_reference in_blades;
  grass_buffer_reference culled_blades;
  draw_command_reference draw_command;
  mat4 view;
  mat4 projection;
  vec4 camera_position; // .xyz = world-space position, .w = optional max render distance
  uint blade_count;
  float total_time;
  float delta_time;
};

bool in_bounds(float value, float bounds) {
  return (value >= -bounds) && (value <= bounds);
}

vec3 random3(vec3 p) {
  return fract(sin(vec3(dot(p, vec3(127.1, 311.7, 513.76)), dot(p, vec3(269.5, 183.3, 389.22)), dot(p, vec3(378.1, 210.4, 193.9)))) * 43758.5453);
}


void main() {
  // Reset the number of blades to 0
  if (gl_GlobalInvocationID.x == 0) {
    draw_command.vertex_count = 0;
  }

  // Wait till all threads reach this point
  barrier();

  // Apply forces on every blade and update the vertices in the buffer
  // Reference: Responsive Real-Time Grass Rendering for General 3D Scenes
  // https://www.cg.tuwien.ac.at/research/publications/2017/JAHRMANN-2017-RRTG/JAHRMANN-2017-RRTG-draft.pdf
  //
  grass_blade blade       = in_blades.data[gl_GlobalInvocationID.x];
  vec3 v0 = blade.v0.xyz;
  vec3 v1 = blade.v1.xyz;
  vec3 v2 = blade.v2.xyz;
  vec3 up = blade.up.xyz;
  float orientation = blade.v0.w;
  float height = blade.v1.w;
  float width = blade.v2.w;
  float stiffness = blade.up.w;
  vec3 tangent = vec3(cos(orientation), 0.0, sin(orientation));
  vec3 front = cross(tangent, up);

  // gravity
  vec3 environmental_gravity = vec3(0.0, -9.81, 0.0); // environmental gravity for flat plane
  vec3 front_gravity = 0.25 * length(environmental_gravity) * front; // front gravity (let blade bend forward)
  vec3 gravity = environmental_gravity + front_gravity;

  // grass blade's recovery force to natural state
  vec3 iV2 = v0 + height * up;  // natural state
  vec3 recovery = (iV2 - v2) * stiffness;  // collision factor (eta) not involved

  // wind function (for animation)
  vec3 wind = random3(v0) * 3.0 * sin(total_time);
  float f_d = 1 - abs(dot(normalize(wind), normalize(v2 - v0))); // directional influence
  float f_r = dot(v2 - v0, up) / height; // height ratio
  vec3 wind_force = wind * f_d * f_r;

  // total force
  vec3 total_force = recovery + gravity + wind_force;

  // update control points;
  v2 = v2 + delta_time * total_force;

  // --- state validation ---
  // 1. ensure v2 is above ground
  v2 = v2 - up * min(dot(up, v2 - v0), 0.0);

  // 2. ensure the blade of grass always has a slight curvature
  float l_projection = length(v2 - v0 - up * dot(v2 - v0, up));
  v1 = v0 + height * up * max(1 - l_projection / height, 0.05 * max(l_projection / height, 1.0));

  // 3. ensure length of Bezier curve is no larger than the height of blade
  //    use approximation to estimate curve length
  //
  // Reference: GRAVESEN, J. 1993. Adaptive subdivision and the length of Bezier curves. Mathematical Institute, Technical University of Denmark.
  //
  // distance between first & last control point
  float L0 = length(v2 - v0);
  // sum of distance of adjacent control points
  float L1 = length(v2 - v1) + length(v1 - v0);
  // degree of Bezier curve
  const float n = 2.0;
  // estimated curve length
  float L = (2 * L0 + (n - 1) * L1) / (n + 1);

  float r      = height / L;
  vec3 v1_corr = v0 + r * (v1 - v0);
  vec3 v2_corr = v1_corr + r * (v2 - v1);

  v1 = v1_corr;
  v2 = v2_corr;

  // Update blade buffer
  blade.v1.xyz = v1;
  blade.v2.xyz = v2;
  in_blades.data[gl_GlobalInvocationID.x] = blade;

  // Cull blades that are too far away or not in the camera frustum
  // and write them to the culled blades buffer Note: to do this, you will
  // need to use an atomic operation to read and update
  // draw_command.vertexCount You want to write the visible blades to the
  // buffer without write conflicts between threads

  // 1. Orientation test
  vec3 view_dir = normalize(vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)));
  vec3 blade_dir = vec3(cos(orientation), 0.0, sin(orientation));
  bool culled_by_orientation = (abs(dot(view_dir, blade_dir)) < 0.6);

  // 2. View Frustum test
  vec3 midpoint = 0.25 * v0 + 0.5 * v1 + 0.25 * v2;  // midpoint of curve
  const float tolerance = 0.0;
  mat4 view_projection = projection * view;

  vec4 v0_ndc = view_projection * vec4(v0, 1.0);
  float h0 = v0_ndc.w + tolerance;
  bool v0_in = in_bounds(v0_ndc.x, h0) && in_bounds(v0_ndc.y, h0) && in_bounds(v0_ndc.z, h0);

  vec4 v1_ndc = view_projection * vec4(midpoint, 1.0);
  float h1 = v1_ndc.w + tolerance;
  bool v1_in  = in_bounds(v1_ndc.x, h1) && in_bounds(v1_ndc.y, h1) && in_bounds(v1_ndc.z, h1);

  vec4 v2_ndc = view_projection * vec4(v2, 1.0);
  float h2 = v2_ndc.w + tolerance;
  bool v2_in = in_bounds(v2_ndc.x, h2) && in_bounds(v2_ndc.y, h2) && in_bounds(v2_ndc.z, h2);

  bool culled_by_view_frustum = !(v0_in || v1_in || v2_in);

  // 3. Distance Test
  vec3 c = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));  // camera position
  float d_projection = length(v0 - c - up * dot(v0 - c, up));
  const float d_max = 15.0;
  const int num_buckets = 20;
  bool culled_by_distance = ((gl_GlobalInvocationID.x % num_buckets) > floor(num_buckets * (1 - d_projection / d_max)));

  // final output
  if (!culled_by_distance && !culled_by_orientation && !culled_by_view_frustum) {
    culled_blades.data[atomicAdd(draw_command.vertex_count, 1)] = blade;
  }
}
