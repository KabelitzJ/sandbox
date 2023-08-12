#ifndef LIBSBX_SCENES_COMPONENTS_TAG_HPP_
#define LIBSBX_SCENES_COMPONENTS_TAG_HPP_

#include <string>

namespace sbx::scenes {

template<typename Char>
class basic_tag final : public std::basic_string<Char> {

public:

  using super = std::basic_string<Char>;

  template<typename... Args>
  basic_tag(Args&&... args)
  : super{std::forward<Args>(args)...} { }

}; // class tag

using tag = basic_tag<char>;

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_TAG_HPP_
