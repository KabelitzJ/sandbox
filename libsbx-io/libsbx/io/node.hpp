#ifndef LIBSBX_IO_BASIC_NODE_HPP_
#define LIBSBX_IO_BASIC_NODE_HPP_

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
#include <iostream>
#include <initializer_list>

#include <fmt/format.h> 

#include <libsbx/utility/concepts.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/io/concepts.hpp>

namespace sbx::io {

template<typename Type>
concept signed_integer = std::is_integral_v<Type> && !std::is_same_v<Type, bool> && std::is_signed_v<Type>;

template<typename Type>
concept unsigned_integer = std::is_integral_v<Type> && !std::is_same_v<Type, bool> && std::is_unsigned_v<Type>;



template<signed_integer SignedInteger, unsigned_integer UnsignedInteger, std::floating_point Float, typename Boolean, typename String, template<typename> typename List, template<typename, typename> typename Map>
class basic_node {

  using value_type = std::variant<std::monostate, SignedInteger, UnsignedInteger, Float, Boolean, String, List<basic_node>, Map<String, basic_node>>;

public:

  using signed_integer_type = SignedInteger;
  using unsigned_integer_type = UnsignedInteger;
  using floating_point_type = Float;
  using boolean_type = Boolean;
  using string_type = String;
  using list_type = List<basic_node>;
  using map_type = Map<string_type, basic_node>;

  basic_node() 
  : _value{std::monostate{}} { }
  
  template<signed_integer Type>
  requires (std::is_convertible_v<Type, signed_integer_type>)
  basic_node(Type value) 
  : _value{static_cast<signed_integer_type>(value)} { }

  template<unsigned_integer Type>
  requires (std::is_convertible_v<Type, unsigned_integer_type>)
  basic_node(Type value) 
  : _value{static_cast<unsigned_integer_type>(value)} { }

  template<std::floating_point Type>
  requires (std::is_convertible_v<Type, floating_point_type>)
  basic_node(Type value) 
  : _value{static_cast<floating_point_type>(value)} { }

  template<std::same_as<boolean_type> Type>
  basic_node(Type value) 
  : _value{value} { }

  template<std::convertible_to<string_type> Type>
  basic_node(const Type& value) 
  : _value{value} { }

  basic_node(const list_type& value) 
  : _value{value} { }

  basic_node(const map_type& value) 
  : _value{value} { }

  basic_node(const basic_node& other)
  : _value{other._value} { }

  basic_node(basic_node&& other)
  : _value{std::move(other._value)} { }

  ~basic_node() = default;

  auto operator=(const basic_node& other) -> basic_node& {
    _value = other._value;
    return *this;
  }

  auto operator=(basic_node&& other) -> basic_node& {
    _value = std::move(other._value);
    return *this;
  }

  template<signed_integer Type>
  requires (std::is_convertible_v<Type, signed_integer_type>)
  auto operator=(Type value) -> basic_node& {
    _value = value;

    return *this;
  }

  template<unsigned_integer Type>
  requires (std::is_convertible_v<Type, unsigned_integer_type>)
  auto operator=(Type value) -> basic_node& {
    _value = value;

    return *this;
  }

  template<std::floating_point Type>
  requires (std::is_convertible_v<Type, floating_point_type>)
  auto operator=(Type value) -> basic_node& {
    _value = value;

    return *this;
  }

  template<std::same_as<boolean_type> Type>
  auto operator=(Type value) -> basic_node& {
    _value = value;

    return *this;
  }

  template<std::convertible_to<string_type> Type>
  auto operator=(const Type& value) -> basic_node& {
    _value = value;

    return *this;
  }

  auto operator=(string_type&& value) -> basic_node& {
    _value = std::move(value);

    return *this;
  }

  auto operator=(const list_type& value) -> basic_node& {
    _value = value;

    return *this;
  }

  auto operator=(const map_type& value) -> basic_node& {
    _value = value;

    return *this;
  }

  template<serializable<basic_node> Type>
  auto operator=(const Type& value) -> basic_node& {
    return operator<<(*this, value);
  }

  auto operator[](const string_type& key) -> basic_node& {
    if (!std::holds_alternative<map_type>(_value)) {
      _value = map_type{};
    }

    auto& map = std::get<map_type>(_value);

    return map[key];
  }

  template<typename Type>
  auto push_back(const Type& value) -> void {
    if (!std::holds_alternative<list_type>(_value)) {
      throw std::runtime_error{"Node is not a list"};
    }

    auto& list = std::get<list_type>(_value);

    list.push_back(value);
  }

  template<typename Type>
  auto push_back(Type&& value) const -> void {
    if (!std::holds_alternative<list_type>(_value)) {
      throw std::runtime_error{"Node is not a list"};
    }

    auto& list = std::get<list_type>(_value);

    list.push_back(std::forward<Type>(value));
  }

  auto type() -> std::string {
    if (std::holds_alternative<signed_integer_type>(_value)) {
      return "signed integer";
    } else if (std::holds_alternative<unsigned_integer_type>(_value)) {
      return "unsigned integer";
    } else if (std::holds_alternative<floating_point_type>(_value)) {
      return "floating point";
    } else if (std::holds_alternative<boolean_type>(_value)) {
      return "boolean";
    } else if (std::holds_alternative<string_type>(_value)) {
      return "string";
    } else if (std::holds_alternative<list_type>(_value)) {
      return "list";
    } else if (std::holds_alternative<map_type>(_value)) {
      return "map";
    }

    return "null";
  }

  template<typename Type>
  auto as() -> Type& {
    if (std::holds_alternative<std::monostate>(_value)) {
      throw std::runtime_error{"Invalid type conversion"};
    }

    if (!std::holds_alternative<Type>(_value)) {
      throw std::runtime_error{fmt::format("Invalid type conversion from {} to {}", type, typeid(Type).name())};
    }

    return std::get<Type>(_value);
  }

  auto to_string() const -> std::string {
    return _to_string(0);
  }

  static auto from_string(const std::string& string) -> basic_node {
    return basic_node{};
  }

private:

  auto _to_string(std::uint32_t indent_level = 0) const -> std::string {
    if (std::holds_alternative<std::monostate>(_value)) {
      return std::string{};
    }

    auto string = std::stringstream{};

    // [NOTE] KAJ 2023-02-01 22:54 - I hate that I cant use "std::string{indent_level, ' '}" here :(
    const auto indent = std::string(indent_level, ' ');

    if (std::holds_alternative<map_type>(_value)) {
      const auto& map = std::get<map_type>(_value);

      for (const auto& [name, child] : map) {
        const auto is_child_of_primitive_type = !std::holds_alternative<map_type>(child._value) && !std::holds_alternative<list_type>(child._value);
        if (is_child_of_primitive_type) {
          string << indent << name << ": " << child._to_string();
        } else {
          string << indent << name << ":\n";
          string << child._to_string(indent_level + 2);
        }
      }
    } else if (std::holds_alternative<list_type>(_value)) {
      const auto& list = std::get<list_type>(_value);
      
      for (const auto& child : list) {
        string << indent << "- " << child._to_string();
      }
    } else {
      string << indent;

      if (std::holds_alternative<signed_integer_type>(_value)) {
        string << std::get<signed_integer_type>(_value) << "\n";
      } else if (std::holds_alternative<unsigned_integer_type>(_value)) {
        string << std::get<unsigned_integer_type>(_value) << "\n";
      } else if (std::holds_alternative<floating_point_type>(_value)) {
        string << std::get<floating_point_type>(_value) << "\n";
      } else if (std::holds_alternative<boolean_type>(_value)) {
        string << (std::get<boolean_type>(_value) ? "true" : "false") << "\n";
      } else if (std::holds_alternative<string_type>(_value)) {
        string << '\"' << std::get<string_type>(_value) << "\"\n";
      }
    }

    return string.str();
  }

  // A method that checks if a string is an unsigned integer
  auto _is_unsigned_integer(const std::string& string) const noexcept -> bool {
    if (string.empty()) {
      return false;
    }

    return std::ranges::all_of(string, [](const auto& character) {
      return std::isdigit(character);
    });
  }

  // Create a method that checks if a string is a signed integer
  auto _is_signed_integer(const std::string& string) const noexcept -> bool {
    if (string.empty()) {
      return false;
    }

    if (string[0] == '-') {
      return _is_unsigned_integer(string.substr(1));
    }

    return _is_unsigned_integer(string);
  }

  // Create a method that checks if a string is a floating point number
  auto _is_floating_point(const std::string& string) const noexcept -> bool {
    auto decimal_point_position = string.find('.');

    if (decimal_point_position == std::string::npos) {
      return false;
    }

    return _is_signed_integer(string.substr(0, decimal_point_position)) && _is_unsigned_integer(string.substr(decimal_point_position + 1));
  }

  // Create a method that checks if a string is a boolean
  auto _is_boolean(const std::string& string) const noexcept -> bool {
    return string == "true" || string == "false";
  }

  // Create a method that checks if a string is a string
  auto _is_string(const std::string& string) const noexcept -> bool {
    return string[0] == '\"' && string[string.size() - 1] == '\"';
  }

  auto _holds_primitive_type(const basic_node& node) const noexcept {
    return !std::holds_alternative<map_type>(node._value) && !std::holds_alternative<list_type>(node._value);
  }

  value_type _value{};

}; // class basic_node

using node = basic_node<std::int32_t, std::uint32_t, std::float_t, bool, std::string, std::vector, std::unordered_map>;

using ordered_node = basic_node<std::int32_t, std::uint32_t, std::float_t, bool, std::string, std::vector, std::map>;

} // namespace sbx::io

#endif // LIBSBX_IO_BASIC_NODE_HPP_
