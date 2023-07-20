#ifndef LIBSBX_SCENES_COMPONENTS_ID_HPP_
#define LIBSBX_SCENES_COMPONENTS_ID_HPP_

#include <libsbx/math/uuid.hpp>

namespace sbx::scenes {

class id {

public:

  id(math::uuid id)
  : _id{id} { }

  id(const id& other) = default;

  operator const math::uuid&() const noexcept {
    return _id;
  }

  operator math::uuid&() noexcept {
    return _id;
  }

  auto value() const noexcept -> math::uuid {
    return _id;
  }

private:

  math::uuid _id;

}; // class id

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_ID_HPP_
