#ifndef LIBSBX_IO_basic_node_HPP_
#define LIBSBX_IO_basic_node_HPP_

#include <cinttypes>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <type_traits>
#include <variant>
#include <vector>
#include <sstream>
#include <optional>

#include <fmt/format.h> 

#include <libsbx/utility/concepts.hpp>

namespace sbx::io {

template<typename... Types>
concept node_value = (
  std::is_integral_v<Types...> || 
  std::is_floating_point_v<Types...> || 
  std::is_same_v<Types..., std::string>
) && utility::are_all_unique_v<Types...>;

template<node_value... Types>
class basic_node {

  using self_type = basic_node<Types...>;
  using variant_type = std::variant<Types...>;

public:

  basic_node() = default;

  // template<utility::convertible_to_one_of<Types...> Type>
  // basic_node(const Type& value)
  // : _value{value} { }

  ~basic_node() = default;

  // template<utility::one_of<Types...> Type>
  // auto as() -> Type& {
  //   if (!_value) {
  //     throw std::runtime_error{"Node has no value"};
  //   }

  //   if (!std::holds_alternative<Type>(_value.value())) {
  //     throw std::runtime_error{"Node holds different type"};
  //   }

  //   return std::get<Type>(_value.value());
  // }

  // auto at(const std::string& name) -> self_type& {
  //   if (auto entry = _indices.find(name); entry != _indices.end()) {
  //     return _children.at(entry->second).second;
  //   } 
    
  //   throw std::runtime_error{fmt::format("Node has no child named '{}'", name)};
  // }

  // auto operator[](const std::string& name) -> self_type& {
  //   if (auto entry = _indices.find(name); entry != _indices.end()) {
  //     return _children.at(entry->second).second;
  //   }

  //   _indices.insert({name, _children.size()});
  //   _children.push_back({name, self_type{name}});

  //   return _children.back().second;
  // }

private:



}; // class basic_node

template<typename... Types>
std::ostream& operator<<(std::ostream& output_stream, const basic_node<Types...>& node) {
  return output_stream << node.to_string();
}

template<typename... Types>
std::ifstream& operator>>(std::ifstream& input_stream, basic_node<Types...>& node) {
  return input_stream;
}

using node = basic_node<std::string, std::int32_t, std::uint32_t, std::float_t>;

} // namespace sbx::io

#endif // LIBSBX_IO_basic_node_HPP_
