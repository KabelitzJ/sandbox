#version 450

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct vertex {
  vec3 position;
  vec3 normal;
  vec2 uv;
};

layout(binding = 0) uniform uniform_parameters {
  uint subdivisions;
} parameters;

layout(binding = 1, std430) buffer buffer_out_vertices {
  vertex data[];
} out_vertices;

layout(binding = 2, std430) buffer buffer_out_indices {
  uint data[];
} out_indices;

const float PI = 3.14159265359;
const float PHI = (1.0 + sqrt(5.0)) * 0.5;  // Golden ratio

const vec3 VERTICES[12] = vec3[](
  vec3(-1.0,  PHI,  0.0),
  vec3( 1.0,  PHI,  0.0),
  vec3(-1.0, -PHI,  0.0),
  vec3( 1.0, -PHI,  0.0),
  vec3( 0.0, -1.0,  PHI),
  vec3( 0.0,  1.0,  PHI),
  vec3( 0.0, -1.0, -PHI),
  vec3( 0.0,  1.0, -PHI),
  vec3( PHI,  0.0, -1.0),
  vec3( PHI,  0.0,  1.0),
  vec3(-PHI,  0.0, -1.0),
  vec3(-PHI,  0.0,  1.0)
);

const int INDICES[60] = int[](
  0, 11,  5, 0,  5,  1,  0,  1,  7,  0,  7, 10, 0, 10, 11,
  1,  5,  9, 5, 11,  4, 11, 10,  2, 10,  7,  6, 7,  1,  8,
  3,  9,  4, 3,  4,  2,  3,  2,  6,  3,  6,  8, 3,  8,  9,
  4,  9,  5, 2,  4, 11,  6,  2, 10,  8,  6,  7, 9,  8,  1
);  

vec2 spherical_uv(vec3 position) {
  float u = atan(position.z, position.x) / (2.0 * PI) + 0.5;
  float v = asin(position.y) / PI + 0.5;
  return vec2(u, v);
}

uint add_vertex(vec3 position, inout int vertex_count) {
  vec3 normalized_position = normalize(position);

  out_vertices.data[vertex_count].position = normalized_position;
  out_vertices.data[vertex_count].normal = normalized_position;  // For a sphere, normal is the same as position
  out_vertices.data[vertex_count].uv = spherical_uv(normalized_position);

  return vertex_count++;
}

void subdivide(inout int index_count, inout int vertex_count) {
  uint indices_temp[60];
  int current_indices = index_count;

  for (int i = 0; i < current_indices; i += 3) {
    uint v1 = out_indices.data[i];
    uint v2 = out_indices.data[i + 1];
    uint v3 = out_indices.data[i + 2];

    // Calculate midpoints and add new vertices
    uint v12 = add_vertex(mix(out_vertices.data[v1].position, out_vertices.data[v2].position, 0.5), vertex_count);
    uint v23 = add_vertex(mix(out_vertices.data[v2].position, out_vertices.data[v3].position, 0.5), vertex_count);
    uint v31 = add_vertex(mix(out_vertices.data[v3].position, out_vertices.data[v1].position, 0.5), vertex_count);

    // Create new indices
    indices_temp[index_count++] = v1;
    indices_temp[index_count++] = v12;
    indices_temp[index_count++] = v31;

    indices_temp[index_count++] = v2;
    indices_temp[index_count++] = v23;
    indices_temp[index_count++] = v12;

    indices_temp[index_count++] = v3;
    indices_temp[index_count++] = v31;
    indices_temp[index_count++] = v23;

    indices_temp[index_count++] = v12;
    indices_temp[index_count++] = v23;
    indices_temp[index_count++] = v31;
  }

  // Copy temporary indices back
  for (int i = 0; i < index_count; i++) {
    out_indices.data[i] = indices_temp[i];
  }
}

void main() {
  int vertex_count = 12;
  int index_count = 60;

  // Initialize vertices with positions, normals, and UVs
  for (int i = 0; i < 12; i++) {
    out_vertices.data[i].position = normalize(VERTICES[i]);
    out_vertices.data[i].normal = out_vertices.data[i].position;
    out_vertices.data[i].uv = spherical_uv(out_vertices.data[i].position);
  }

  // Initialize indices
  for (int i = 0; i < 60; i++) {
    out_indices.data[i] = INDICES[i];
  }

  // Subdivide based on the level
  for (int i = 0; i < 1; i++) {
    subdivide(index_count, vertex_count);
  }
}
