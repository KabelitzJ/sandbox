#ifndef LIBSBX_GRAPHICS_MODEL_HPP_
#define LIBSBX_GRAPHICS_MODEL_HPP_

#include <filesystem>

#include <tiny_obj_loader.h>

#include <libsbx/graphics/mesh.hpp>
#include <libsbx/graphics/material.hpp>

namespace sbx::graphics {

class model {

public:

  model(const std::filesystem::path& path);

  ~model() = default;

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  auto mesh() const noexcept -> const class mesh& {
    return *_mesh;
  }

  auto material() const noexcept -> const class material& {
    return *_material;
  }

private:

  std::string _name{};
  std::unique_ptr<graphics::mesh> _mesh;
  std::unique_ptr<graphics::material> _material;

}; // class model

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_MODEL_HPP_
