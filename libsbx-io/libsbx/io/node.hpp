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
#include <iostream>
#include <initializer_list>

#include <fmt/format.h> 

#include <libsbx/utility/concepts.hpp>

#include <libsbx/io/concepts.hpp>

namespace sbx::io {

template<typename Container, typename Key, typename Value>
concept associative_container = requires(Container& container, const Key& key, const Value& value) {
  { Container::key_type } -> std::same_as<Key>;
  { Container::mapped_type } -> std::same_as<Value>;
  { Container::value_type } -> std::same_as<std::pair<const Key, Value>>;
  { container.begin() } -> std::forward_iterator;
  { container.end() } -> std::forward_iterator;
  { container[key] } -> std::same_as<Value&>;
}; // concept associative_container



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

  basic_node(signed_integer_type value) 
  : _value{value} { }

  template<signed_integer Type>
  requires (std::is_convertible_v<Type, signed_integer_type>)
  basic_node(Type value) 
  : _value{static_cast<signed_integer_type>(value)} { }

  basic_node(unsigned_integer_type value) 
  : _value{value} { }

  template<unsigned_integer Type>
  requires (std::is_convertible_v<Type, unsigned_integer_type>)
  basic_node(Type value) 
  : _value{static_cast<unsigned_integer_type>(value)} { }

  basic_node(floating_point_type value) 
  : _value{value} { }

  template<std::floating_point Type>
  requires (std::is_convertible_v<Type, floating_point_type>)
  basic_node(Type value) 
  : _value{static_cast<floating_point_type>(value)} { }

  template<std::same_as<boolean_type> Type>
  basic_node(Type value) 
  : _value{value} { }

  basic_node(const string_type& value) 
  : _value{value} { }

  basic_node(const list_type& value) 
  : _value{value} { }

  basic_node(const map_type& value) 
  : _value{value} { }

  ~basic_node() = default;

  auto operator=(signed_integer_type value) -> basic_node& {
    _value = value;

    return *this;
  }

  auto operator=(unsigned_integer_type value) -> basic_node& {
    _value = value;

    return *this;
  }

  auto operator=(floating_point_type value) -> basic_node& {
    _value = value;

    return *this;
  }

  template<std::same_as<boolean_type> Type>
  auto operator=(Type value) -> basic_node& {
    _value = value;

    return *this;
  }

  auto operator=(const string_type& value) -> basic_node& {
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

  auto at(const string_type& key) -> basic_node& {
    if (!std::holds_alternative<map_type>(_value)) {
      throw std::runtime_error{"Tried to access node that is not of type map"};
    }

    return std::get<map_type>(_value).at(key);
  }

  auto operator[](const string_type& key) -> basic_node& {
    if (!std::holds_alternative<map_type>(_value)) {
      _value = map_type{};
    }

    auto& map = std::get<map_type>(_value);

    return map[key];
  }

  auto to_string(std::uint32_t indent_level = 0) const -> std::string {
    if (std::holds_alternative<std::monostate>(_value)) {
      return std::string{};
    }

    auto string = std::stringstream{};

    // [NOTE] KAJ 2023-02-01 22:54 - I hate that I cant use "std::string{indent_level, ' '}" here :(
    const auto indent = std::string(indent_level, ' ');

    if (std::holds_alternative<map_type>(_value)) {
      const auto& map = std::get<map_type>(_value);

      for (const auto& [name, child] : map) {
        if (_holds_primitive_type(child)) {
          string << indent << name << ": " << child.to_string();
        } else {
          string << indent << name << ":\n";
          string << child.to_string(indent_level + 2);
        }
      }
    } else if (std::holds_alternative<list_type>(_value)) {
      const auto& list = std::get<list_type>(_value);
      
      for (const auto& child : list) {
        string << indent << "- " << child.to_string();
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

private:

  auto _holds_primitive_type(const basic_node& node) const noexcept {
    return !std::holds_alternative<map_type>(node._value) && !std::holds_alternative<list_type>(node._value);
  }

  value_type _value{};

}; // class basic_node

template<signed_integer SignedInteger, unsigned_integer UnsignedInteger, std::floating_point Float, typename Boolean, typename String, template<typename> typename List, template<typename, typename> typename Map>
std::ostream& operator<<(std::ostream& output_stream, const basic_node<SignedInteger, UnsignedInteger, Float, Boolean, String, List, Map>& node) {
  return output_stream << node.to_string();
}

template<signed_integer SignedInteger, unsigned_integer UnsignedInteger, std::floating_point Float, typename Boolean, typename String, template<typename> typename List, template<typename, typename> typename Map>
std::istream& operator>>(std::istream& input_stream, basic_node<SignedInteger, UnsignedInteger, Float, Boolean, String, List, Map>& node) {
  return input_stream;
}

using node = basic_node<std::int32_t, std::uint32_t, std::float_t, bool, std::string, std::vector, std::unordered_map>;

using ordered_node = basic_node<std::int32_t, std::uint32_t, std::float_t, bool, std::string, std::vector, std::map>;

} // namespace sbx::io

#endif // LIBSBX_IO_basic_node_HPP_
