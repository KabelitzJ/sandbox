#ifndef LIBSBX_IO_basic_node_HPP_
#define LIBSBX_IO_basic_node_HPP_

#include <cinttypes>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <map>
#include <type_traits>
#include <variant>
#include <vector>
#include <sstream>
#include <optional>

#include <fmt/format.h> 

#include <libsbx/utility/concepts.hpp>

namespace sbx::io {

template<bool Ordered>
class basic_node {

  using map_type = std::conditional_t<Ordered, std::map<std::string, basic_node>, std::unordered_map<std::string, basic_node>>;

  using array_type = std::vector<basic_node>;

  using value_type = std::variant<std::monostate, std::int32_t, std::float_t, bool, std::string, array_type, map_type>;


public:

  template<typename Type>
  inline static constexpr auto is_valid_type_v = utility::one_of<Type, std::int32_t, std::float_t, bool, std::string, array_type, map_type>;

  inline static constexpr auto is_ordered = Ordered;

  basic_node() = default;

  template<typename Type>
  requires (is_valid_type_v<Type>)
  basic_node(const Type& value) 
  : _value{value} { }

  template<typename Type>
  requires (is_valid_type_v<Type>)
  basic_node(Type&& value) 
  : _value{std::move(value)} { }

  ~basic_node() = default;

  auto operator[](const std::string& key) -> basic_node& {
    if (!std::holds_alternative<map_type>(_value)) {
      _value = map_type{};
    }

    return std::get<map_type>(_value)[key];
  }

  auto operator[](std::size_t index) -> basic_node& {
    if (!std::holds_alternative<array_type>(_value)) {
      _value = array_type{};
    }
    
    return std::get<array_type>(_value)[index];
  }

  template<typename Type>
  requires (is_valid_type_v<Type>)
  auto push_back(const Type& value) -> void {
    if (!std::holds_alternative<array_type>(_value)) {
      throw std::runtime_error{"Tried to push into non array type node"};
    }

    std::get<array_type>(_value).push_back(basic_node{value});
  }

  template<typename Type>
  requires (is_valid_type_v<Type>)
  auto operator=(const Type& value) -> basic_node& {
    _value = value;
    return *this;
  }

  auto to_string(std::uint32_t indent_level = 0) const -> std::string {
    auto string = std::stringstream{};

    if (std::holds_alternative<map_type>(_value)) {
      for (const auto& [name, child] : std::get<map_type>(_value)) {
        string << std::string(indent_level, ' ') << name << ":\n";
        string << child.to_string(indent_level + 2);
      }
    } else if (std::holds_alternative<array_type>(_value)) {
      auto& array = std::get<array_type>(_value);
      
      if (array.empty()) {
        string << std::string(indent_level, ' ') << "- []\n";
      } else {
        for (const auto& child : array) {
          string << std::string(indent_level, ' ') << "-\n";
          string << child.to_string(indent_level + 2);
        }
      }
    } else {
      string << std::string(indent_level, ' ');

      if (_value.index() == 1) {
        string << "- " << std::get<std::int32_t>(_value) << "\n";
      } else if (_value.index() == 2) {
        string << "- " << std::get<std::float_t>(_value) << "\n";
      } else if (_value.index() == 3) {
        string << "- " << (std::get<bool>(_value) ? "yes" : "no") << "\n";
      } else if (_value.index() == 4) {
        string << "- \"" << std::get<std::string>(_value) << "\"\n";
      }
    }

    return string.str();
  }

private:

  value_type _value{};

}; // class basic_node

template<bool Ordered>
std::ostream& operator<<(std::ostream& output_stream, const basic_node<Ordered>& node) {
  return output_stream << node.to_string();
}

template<bool Ordered>
std::istream& operator>>(std::istream& input_stream, basic_node<Ordered>& node) {
  return input_stream;
}

using unordered_node = basic_node<false>;

using ordered_node = basic_node<true>;

using node = unordered_node;

} // namespace sbx::io

#endif // LIBSBX_IO_basic_node_HPP_
