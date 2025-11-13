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

template<>
struct std::hash<sbx::scenes::selection_tag> {
  auto operator()(const sbx::scenes::selection_tag& selection_tag) const noexcept -> std::size_t {
    return selection_tag.value();
  }
}; // struct std::hash<sbx::math::uuid>

#endif // LIBSBX_SCENES_COMPONENTS_SELECTION_TAG_HPP_