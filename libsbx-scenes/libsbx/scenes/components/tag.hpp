#ifndef LIBSBX_SCENES_COMPONENTS_TAG_HPP_
#define LIBSBX_SCENES_COMPONENTS_TAG_HPP_

#include <string>

namespace sbx::scenes {

class tag {

public:

  tag(const std::string& tag)
  : _tag{tag} { }

  tag(std::string tag)
  : _tag{std::move(tag)} { }

  tag(const tag& other) = default;

  operator const std::string&() const noexcept {
    return _tag;
  }

  operator std::string&() noexcept {
    return _tag;
  }

  auto value() const noexcept -> const std::string& {
    return _tag;
  }

private:

  std::string _tag;

}; // class tag

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_TAG_HPP_
