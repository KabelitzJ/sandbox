#ifndef LIBSBX_SCENES_COMPONENTS_SCRIPT_HPP_
#define LIBSBX_SCENES_COMPONENTS_SCRIPT_HPP_

#include <filesystem>
#include <optional>

#include <sol/sol.hpp>

namespace sbx::scenes {

class script {

public:

  script(const std::filesystem::path& path);

  script(const script& other) = delete;

  script(script&& other) noexcept = default;

  ~script() = default;

  auto operator=(const script& other) -> script& = delete;

  auto operator=(script&& other) noexcept -> script& = default;

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  template<typename Type>
  auto get(const std::string& name) -> Type {
    return _state.get<Type>(name);
  }

  template<typename Type>
  auto set(const std::string& name, const Type& value) -> void {
    _state.set(name, value);
  }

  auto invoke(const std::string& name) -> void;

private:
  
  auto _create_library() -> void;

  std::string _name{};
  sol::state _state{};

}; // class script

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_SCRIPT_HPP_
