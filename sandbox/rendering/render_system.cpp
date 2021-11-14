#include "render_system.hpp"

#include <array>
#include <algorithm>

#include <glad/glad.h>

#include <core/events.hpp>
#include <core/camera.hpp>

#include <types/primitives.hpp>
#include <types/matrix.hpp>
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

  glCreateBuffers(1, &_batch.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void render_system::update([[maybe_unused]] const time delta_time) {
  glClear(GL_COLOR_BUFFER_BIT);

  const auto shader_handle = get_resource<shader>("default_shader");

  shader_handle->bind();

  auto camera_view = create_view<const camera>();

  for (const auto entity : camera_view) {
    auto camera_component = camera_view.get<const camera>(entity);

    if (!camera_component.is_main) {
      continue;
    }
  }

  auto model_view = create_view<const model, const transform>();

  for (const auto entity : model_view) {
    auto [model, transform] = model_view.get(entity);
    
    auto mesh_handle = get_resource<mesh>(model.mesh_id);

    shader_handle->set_matrix4x4("model_matrix", model_matrix_from_transform(transform));

    _add_to_batch(*mesh_handle, transform);

    // [TODO] KAJ 2021-11-14 12:59 - Only temporary... Remove this!
    _flush_batch();
  }

  _flush_batch();

  shader_handle->unbind();
}

void render_system::terminate() {
  
}

void render_system::_add_to_batch(const mesh& mesh, const transform& transform) {
  static_cast<void>(transform);

  if (_batch.vertex_count + mesh.vertices().size() > _batch.vertices.size()) {
    _flush_batch();
  }

  for (const auto& vertex : mesh.vertices()) {
    _batch.vertices[_batch.vertex_count++] = vertex;
  }

  for (const auto index : mesh.indices()) {
    _batch.indices[_batch.index_count++] = index;
  }
}

void render_system::_flush_batch() {
  glBindBuffer(GL_ARRAY_BUFFER, _batch.vertex_buffer);
  const auto vertex_size = static_cast<gl_size_ptr>(_batch.vertex_count * sizeof(mesh_vertex));
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, _batch.vertices.data());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.index_buffer);
  const auto index_size = static_cast<gl_size_ptr>(_batch.index_count * sizeof(uint32));
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index_size, _batch.indices.data());

  glBindVertexArray(_batch.vertex_array);
  glDrawElements(GL_TRIANGLES, static_cast<gl_size>(_batch.index_count), GL_UNSIGNED_INT, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  _reset_batch();
}

void render_system::_reset_batch() {
  std::fill(_batch.vertices.begin(), _batch.vertices.end(), mesh_vertex{});
  _batch.vertex_count = 0u;
  std::fill(_batch.indices.begin(), _batch.indices.end(), uint32{0u});
  _batch.index_count = 0u;
}

} // namespace sbx
