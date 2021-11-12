#include "render_system.hpp"

#include <array>
#include <algorithm>

#include <glad/glad.h>

#include <core/events.hpp>

#include <types/primitives.hpp>
#include <types/gl.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "shader.hpp"

namespace sbx {

render_system::render_system()
: _batch{} {}

void render_system::initialize() {
  glCreateVertexArrays(1, &_batch.vertex_array);
  glBindVertexArray(_batch.vertex_array);

  glCreateBuffers(1, &_batch.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _batch.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, render_batch::max_vertex_count * sizeof(mesh_vertex), nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexArrayAttrib(_batch.vertex_array, 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), reinterpret_cast<const void*>(offsetof(mesh_vertex, position)));

  glEnableVertexArrayAttrib(_batch.vertex_array, 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), reinterpret_cast<const void*>(offsetof(mesh_vertex, normal)));

  glEnableVertexArrayAttrib(_batch.vertex_array, 0);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), reinterpret_cast<const void*>(offsetof(mesh_vertex, uv)));
}

void render_system::update([[maybe_unused]] const time delta_time) {
  glClear(GL_COLOR_BUFFER_BIT);

  auto view = create_view<const model, const transform>();

  for (const auto entity : view) {
    auto [model, transform] = view.get(entity);
    
    for (const auto& mesh : model.meshes()) {
      _add_to_batch(mesh, transform);
    }
  }
}

void render_system::terminate() {
  
}

void render_system::_add_to_batch(const mesh& mesh, const transform& transform) {
  static_cast<void>(mesh);
  static_cast<void>(transform);
}

void render_system::_flush_batch() {
  glBindBuffer(GL_ARRAY_BUFFER, _batch.vertex_buffer);

  _reset_batch();
}

void render_system::_reset_batch() {
  _batch.index_count = 0u;
  _batch.current_vertex = 0u;
  std::fill(_batch.vertices.begin(), _batch.vertices.end(), mesh_vertex{});
}

} // namespace sbx
