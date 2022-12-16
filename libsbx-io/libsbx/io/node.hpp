#ifndef LIBSBX_IO_NODE_HPP_
#define LIBSBX_IO_NODE_HPP_

#include <cinttypes>
#include <cmath>
#include <string>
#include <unordered_map>
#include <variant>

namespace sbx::io {

class node {

public:

  node(const std::int32_t value)
  : _value{value} { }

  node(const std::float_t value)
  : _value{value} { }

  node(const std::string& value)
  : _value{value} { }

  node(const std::unordered_map<std::string, node>& value)
  : _value{value} { }

  ~node() = default;

private:

  std::variant<std::int32_t, std::float_t, std::string, std::unordered_map<std::string, node>> _value{};

}; // class node

} // namespace sbx::io

#endif // LIBSBX_IO_NODE_HPP_
