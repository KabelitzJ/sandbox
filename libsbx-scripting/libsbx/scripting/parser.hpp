#ifndef LIBSBX_SCRIPTING_PARSER_HPP_
#define LIBSBX_SCRIPTING_PARSER_HPP_

#include <fstream>
#include <filesystem>

#include <fmt/format.h>

#include <libsbx/core/logger.hpp>

#include <libsbx/scripting/function.hpp>

namespace sbx::scripting {

class parser {

public:

  struct result {
    std::string name{};
    std::unordered_map<std::string, function> functions{};
  }; // struct result

  parser(const std::filesystem::path& path)
  : _path{path} {
    if (!std::filesystem::exists(path)) {
      throw std::runtime_error{fmt::format("File '{}' does not exist", path.string())};
    }

    if (path.extension() != ".script") {
      throw std::runtime_error{fmt::format("File '{}' is not a valid script file", path.string())};
    }

    _file = std::ifstream{path};

    if (!_file.is_open()) {
      throw std::runtime_error{fmt::format("Could not open file '{}'", path.string())};
    }
  }

  ~parser() = default;

  auto operator()() -> result {
    auto name = _path.stem().string();
    auto functions = std::unordered_map<std::string, function>{};

    return result{
      .name = name,
      .functions = functions
    };
  }

private:

  std::filesystem::path _path{};
  std::ifstream _file{};

}; // class parser

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_PARSER_HPP_
