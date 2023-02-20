#ifndef LIBSBX_IO_YAML_FILE_HPP_
#define LIBSBX_IO_YAML_FILE_HPP_

#include <fstream>

#include <libsbx/io/file.hpp>

#include <fmt/format.h>

#include <yaml-cpp/yaml.h>

namespace sbx::io {

class yaml_file_format {

  template<typename Type>
  friend auto operator<<(yaml_file_format& format, const Type& value) -> yaml_file_format&;

public:

  /**
   * @brief A very thin wrapper around a YAML::Node
   */
  class entry {
  
    friend class yaml_file_format;

  public:

    ~entry() = default;

    template<typename Type>
    auto as() -> Type {
      return _node.as<Type>();
    }

    template<typename Key>
    auto operator[](Key&& key) -> entry {
      return _node[std::forward<Key>(key)];
    }

  private:

    entry(YAML::Node node)
    : _node{node} { }

    YAML::Node _node{};

  }; // class entry

  yaml_file_format(const std::filesystem::path& path)
  : _node{std::make_unique<YAML::Node>(YAML::LoadFile(path.string()))} { }

  template<typename Type>
  auto as() -> Type& {
    return _node->as<Type>();
  }

  template<typename Key>
  auto operator[](Key&& key) -> entry {
    return (*_node)[std::forward<Key>(key)];
  }

private:

  std::unique_ptr<YAML::Node> _node{};

}; // class yaml_file_format

using yaml_file = basic_file<yaml_file_format>;

}; // namespace sbx::io

#endif // LIBSBX_IO_YAML_FILE_HPP_
