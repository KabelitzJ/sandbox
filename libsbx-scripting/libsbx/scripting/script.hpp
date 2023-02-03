#ifndef LIBSBX_SCRIPTING_SCRIPT_HPP_
#define LIBSBX_SCRIPTING_SCRIPT_HPP_

#include <unordered_map>
#include <filesystem>
#include <string>

#include <fmt/format.h>

#include <libsbx/scripting/function.hpp>
#include <libsbx/scripting/parser.hpp>

namespace sbx::scripting {

class script {

public:

  script(const std::filesystem::path& path) { 
    _parse(path);
  }

  ~script() = default;

  template<typename Return, typename... Args>
  auto call_function(const std::string& name, Args&&... args) -> Return {
    if (auto entry = _functions.find(name); entry != _functions.end()) {
      auto& function = entry->second;

      return function.call<Return>(std::forward<Args>(args)...);
    }

    throw std::runtime_error{fmt::format("No function with name '{}' in script '{}'", name, _name)};
  }

private:

  auto _parse(const std::filesystem::path& path) -> void {
    auto script_parser = parser{path};
    
    auto parse_result = script_parser();

    _name = parse_result.name;
    _functions = parse_result.functions;
  }

  std::string _name{};
  std::unordered_map<std::string, function> _functions{};

}; // class script

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPT_HPP_
