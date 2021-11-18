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

}

struct vertex {
  vector3 position{};
  vector2 uv{};
  vector3 normal{};
  // matrix4x4 model_matrix{};
};

void render_system::update([[maybe_unused]] const time delta_time) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  auto model_view = create_view<const model, const transform>();

  for (const auto entity : model_view) {
    const auto [model, transform] = model_view.get(entity);

    auto mesh_handle = get_resource<mesh>(model.mesh_id);

    auto vertices = mesh_handle->vertices();

    auto indices = mesh_handle->indices();

    auto vertex_array = 0u;
    auto vertex_buffer = 0u;
    auto index_buffer = 0u;

    auto shader_handle = get_resource<shader>(model.shader_id);

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(mesh_vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(vertex_array, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), reinterpret_cast<const void*>(offsetof(mesh_vertex, position)));

    glEnableVertexArrayAttrib(vertex_array, 1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), reinterpret_cast<const void*>(offsetof(mesh_vertex, uv)));

    glEnableVertexArrayAttrib(vertex_array, 2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), reinterpret_cast<const void*>(offsetof(mesh_vertex, normal)));

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), indices.data(), GL_STATIC_DRAW);

    shader_handle->bind();

    shader_handle->set_matrix4x4("model_matrix", model_matrix_from_transform(transform));
    shader_handle->set_matrix4x4("view_matrix", look_at(vector3{4.0f, 3.0f, 3.0f}, vector3{0.0f, 0.0f, 0.0f}, vector3{0.0f, 1.0f, 0.0f}));
    shader_handle->set_matrix4x4("projection_matrix", perspective(to_radians(45.0f), 960.0f / 720.0f, 0.1f, 100.0f));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    shader_handle->unbind();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void render_system::terminate() {

}

} // namespace sbx
