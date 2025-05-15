#ifndef LIBSBX_SCENES_COMPONENTS_HEIRARCHY_HPP_
#define LIBSBX_SCENES_COMPONENTS_HEIRARCHY_HPP_

#include <libsbx/scenes/node.hpp>

namespace sbx::scenes {

struct hierarchy {
  node parent{node::null};
  node first_child{node::null};
  node next_sibling{node::null};
  node previous_sibling{node::null};

  hierarchy() = default;

  hierarchy(const node parent)
  : parent{parent} { }

}; // struct hierarchy

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_HEIRARCHY_HPP_
