#ifndef LIBSBX_GRAPHICS_MODEL_HPP_
#define LIBSBX_GRAPHICS_MODEL_HPP_

#include <memory>

#include <libsbx/assets/asset.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

namespace sbx::graphics {

class model {

public:

  model(const assets::asset_id mesh_id, const assets::asset_id material_id)
  : _mesh_id{mesh_id},
    _material_id{material_id} { }

  ~model() = default;

  auto mesh_id() const noexcept -> assets::asset_id {
    return _mesh_id;
  }

  auto material_id() const noexcept -> assets::asset_id {
    return _material_id;
  }

private:

  assets::asset_id _mesh_id;
  assets::asset_id _material_id;

}; // class model

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_MODEL_HPP_
