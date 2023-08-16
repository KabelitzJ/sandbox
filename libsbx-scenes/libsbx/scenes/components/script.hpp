#ifndef LIBSBX_SCENES_COMPONENTS_SCRIPT_HPP_
#define LIBSBX_SCENES_COMPONENTS_SCRIPT_HPP_

#include <libsbx/assets/asset.hpp>

namespace sbx::scenes {

class script : public assets::asset_id {

public:

  using super = assets::asset_id;

  template<typename... Args>
  script(Args&&... args)
  : super{std::forward<Args>(args)...} { }

}; // class script

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_SCRIPT_HPP_
