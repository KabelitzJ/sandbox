#include "render_system.hpp"

#include <array>
#include <algorithm>

#include <glad/glad.h>

#include <core/events.hpp>
#include <core/camera.hpp>
#include <core/logger.hpp>

#include <types/vector.hpp>
#include <types/matrix.hpp>
#include <types/transform.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "shader.hpp"

namespace sbx {

render_system::render_system() { }

void render_system::initialize() {

  glGenVertexArrays(1, &_batch.vertex_array);
  glBindVertexArray(_batch.vertex_array);

  glGenBuffers(1, &_batch.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _batch.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * render_batch::max_element_count, nullptr, GL_DYNAMIC_DRAW);

  const auto position_offset = reinterpret_cast<const void*>(offsetof(vertex, position));
  const auto uv_offset = reinterpret_cast<const void*>(offsetof(vertex, uv));
  const auto normal_offset = reinterpret_cast<const void*>(offsetof(vertex, normal));
  const auto model_matrix1_offset = reinterpret_cast<const void*>(offsetof(vertex, model_matrix));
  const auto model_matrix2_offset = reinterpret_cast<const void*>(offsetof(vertex, model_matrix) + sizeof(vector4));
  const auto model_matrix3_offset = reinterpret_cast<const void*>(offsetof(vertex, model_matrix) + sizeof(vector4) * 2);
  const auto model_matrix4_offset = reinterpret_cast<const void*>(offsetof(vertex, model_matrix) + sizeof(vector4) * 3);

  glEnableVertexArrayAttrib(_batch.vertex_array, 0); // position
  glVertexAttribPointer(_batch.vertex_array, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), position_offset);

  glEnableVertexArrayAttrib(_batch.vertex_array, 1); // uv
  glVertexAttribPointer(_batch.vertex_array, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), uv_offset);

  glEnableVertexArrayAttrib(_batch.vertex_array, 2); // normal
  glVertexAttribPointer(_batch.vertex_array, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), normal_offset);

  glEnableVertexArrayAttrib(_batch.vertex_array, 3); // model_matrix1
  glVertexAttribPointer(_batch.vertex_array, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), model_matrix1_offset);

  glEnableVertexArrayAttrib(_batch.vertex_array, 4); // model_matrix2
  glVertexAttribPointer(_batch.vertex_array, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), model_matrix2_offset);

  glEnableVertexArrayAttrib(_batch.vertex_array, 5); // model_matrix3
  glVertexAttribPointer(_batch.vertex_array, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), model_matrix3_offset);

  glEnableVertexArrayAttrib(_batch.vertex_array, 6); // model_matrix4
  glVertexAttribPointer(_batch.vertex_array, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), model_matrix4_offset);

  glGenBuffers(1, &_batch.index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gl_index) * render_batch::max_element_count, nullptr, GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  _reset_batch();
}

void render_system::update([[maybe_unused]] const time delta_time) {
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(_batch.vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, _batch.vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.index_buffer);

  auto shader_handle = get_resource<shader>("default_shader");

  shader_handle->bind();

  auto camera_view = create_view<const camera>();

  for (const auto entity : camera_view) {
    const auto& camera_component = camera_view.get<const camera>(entity);

    if (!camera_component.is_main) {
      continue;
    }

    shader_handle->set_matrix4x4("uniforms.view_matrix", camera_component.view_matrix);
    shader_handle->set_matrix4x4("uniforms.projection_matrix", camera_component.projection_matrix);
  }

  auto model_view = create_view<const model, const transform>();

  for (const auto entity : model_view) {
    const auto [model_component, transform_component] = model_view.get<const model, const transform>(entity);

    auto mesh_handle = get_resource<mesh>(model_component.mesh_id);

    _add_to_batch(*mesh_handle, transform_component);
  }

  _flush_batch();

  shader_handle->unbind();

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render_system::terminate() {
  glDeleteBuffers(1, &_batch.vertex_buffer);
  glDeleteBuffers(1, &_batch.index_buffer);
  glDeleteVertexArrays(1, &_batch.vertex_array);
}

void render_system::_reset_batch() {
  std::fill(_batch.vertices.begin(), _batch.vertices.end(), vertex{});
  _batch.vertex_count = 0u;
  
  std::fill(_batch.indices.begin(), _batch.indices.end(), gl_index{});
  _batch.index_count = 0u;
}

void render_system::_add_to_batch(const mesh& mesh, const transform& transform) {
  if (_batch.index_count + mesh.indices().size() > render_batch::max_element_count) {
    _flush_batch();
  }

  const auto model_matrix = model_matrix_from_transform(transform);

  for (const auto& mesh_vertex : mesh.vertices()) {
    const auto temp = vertex{mesh_vertex.position, mesh_vertex.uv, mesh_vertex.normal, model_matrix};
    _batch.vertices[_batch.vertex_count] = temp;
    ++_batch.vertex_count;
  }

  for (const auto mesh_index : mesh.indices()) {
    _batch.indices[_batch.index_count] = mesh_index;
    ++_batch.index_count;
  }
}

void render_system::_flush_batch() {
  if (_batch.vertex_count == 0u || _batch.index_count == 0u) {
    return;
  }

  glBindVertexArray(_batch.vertex_array);

  glBindBuffer(GL_ARRAY_BUFFER, _batch.vertex_buffer);
  const auto vertex_size = static_cast<gl_size_ptr>(sizeof(vertex) * _batch.vertex_count);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, _batch.vertices.data());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.index_buffer);
  const auto index_size = static_cast<gl_size_ptr>(sizeof(gl_index) * _batch.index_count);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index_size, _batch.indices.data());

  const auto indices_to_draw = static_cast<gl_size>(_batch.index_count);
  glDrawElements(GL_TRIANGLES, indices_to_draw, GL_UNSIGNED_INT, nullptr);


  glBindVertexArray(0);

  _reset_batch();
}

} // namespace sbx
