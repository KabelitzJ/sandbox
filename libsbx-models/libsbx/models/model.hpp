#ifndef LIBSBX_MODELS_MODEL_HPP_
#define LIBSBX_MODELS_MODEL_HPP_

#include <filesystem>

#include <tiny_obj_loader.h>

#include <libsbx/models/mesh.hpp>
#include <libsbx/models/material.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

namespace sbx::models {

class model {

public:

  model(const std::filesystem::path& path);

  ~model() = default;

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  auto mesh() const noexcept -> const class models::mesh& {
    return *_mesh;
  }

  auto material() const noexcept -> const class models::material& {
    return *_material;
  }

  auto render(graphics::command_buffer& command_buffer, std::float_t delta_time) -> void {
    _mesh->render(command_buffer, delta_time);
  }

private:

  std::string _name{};

  std::unique_ptr<models::mesh> _mesh;
  std::unique_ptr<models::material> _material;

}; // class model

} // namespace sbx::models

#endif // LIBSBX_MODELS_MODEL_HPP_
