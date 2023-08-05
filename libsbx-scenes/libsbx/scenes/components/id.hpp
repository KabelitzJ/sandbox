#ifndef LIBSBX_SCENES_COMPONENTS_ID_HPP_
#define LIBSBX_SCENES_COMPONENTS_ID_HPP_

#include <libsbx/math/uuid.hpp>

namespace sbx::scenes {

class id final {

public:

  id()
  : _value{} { }

  id(math::uuid value)
  : _value{value} { }

  operator math::uuid() const noexcept {
    return _value;
  }

private:

  math::uuid _value;

}; // class id

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_ID_HPP_
