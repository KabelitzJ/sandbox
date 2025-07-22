#ifndef LIBSBX_SCENES_COMPONENTS_SELECTION_TAG_HPP_
#define LIBSBX_SCENES_COMPONENTS_SELECTION_TAG_HPP_

#include <libsbx/math/uuid.hpp>

namespace sbx::scenes {

class selection_tag final : public math::uuid {

  using base = math::uuid;

public:

  inline static const auto null = base::null();

  template<typename... Args>
  selection_tag(Args&&... args)
  : base{std::forward<Args>(args)...} { }

}; // class selection_tag

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_SELECTION_TAG_HPP_