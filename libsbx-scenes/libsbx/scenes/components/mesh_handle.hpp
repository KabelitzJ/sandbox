#ifndef LIBSBX_SCENES_COMPONENTS_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_MESH_HPP_

#include <libsbx/models/mesh.hpp>

namespace sbx::scenes {

class mesh_handle {

public:

  using handle_type = models::mesh::handle_type;

  mesh_handle(handle_type handle)
  : _handle{handle} { }

  mesh_handle(const mesh_handle& other) = default;

  operator const handle_type&() const noexcept {
    return _handle;
  }

  operator handle_type&() noexcept {
    return _handle;
  }

  auto value() const noexcept -> const handle_type& {
    return _handle;
  }

private:

  handle_type _handle;

}; // class mesh_handle

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_MESH_HPP_
