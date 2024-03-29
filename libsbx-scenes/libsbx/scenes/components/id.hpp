#ifndef LIBSBX_SCENES_COMPONENTS_ID_HPP_
#define LIBSBX_SCENES_COMPONENTS_ID_HPP_

#include <libsbx/math/uuid.hpp>

namespace sbx::scenes {

class id final : public math::uuid {

public:

  using super = math::uuid;

  template<typename... Args>
  id(Args&&... args)
  : super{std::forward<Args>(args)...} { }

}; // class id

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_ID_HPP_
