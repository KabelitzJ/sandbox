#ifndef LIBSBX_SCENES_COMPONENTS_TAG_HPP_
#define LIBSBX_SCENES_COMPONENTS_TAG_HPP_

#include <string>

namespace sbx::scenes {

class tag final : public std::string {

public:

  using super = std::string;

  using super::super;

}; // class tag

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_TAG_HPP_
