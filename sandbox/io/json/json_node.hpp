#ifndef SBX_IO_JSON_NODE_HPP_
#define SBX_IO_JSON_NODE_HPP_

#include <memory>
#include <variant>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <types/primitives.hpp>

namespace sbx {

class json_node;

using json_object = std::unordered_map<std::string, std::shared_ptr<json_node>>;
using json_array = std::vector<std::shared_ptr<json_node>>;

class json_node {

 friend std::ostream& operator<<(std::ostream& os, const json_node& node);

public:

  using size_type = std::size_t;

  json_node()
  : _value{},
    _type{type::null} { }

  json_node(const std::shared_ptr<json_object>& object)
  : _value{object},
    _type{type::object} { }

  json_node(const std::shared_ptr<json_array>& array)
  : _value{array},
    _type{type::array} { }

  json_node(const std::string& string)
  : _value{string},
    _type{type::string} { }

  json_node(const sbx::float32 number)
  : _value{number},
    _type{type::number} { }

  template<typename Type>
  requires (std::is_same_v<Type, bool>)
  json_node(const Type value)
  : _value{value},
    _type{type::boolean} { }

  bool is_object() const noexcept {
    return _type == type::object;
  }

  bool is_array() const noexcept {
    return _type == type::array;
  }

  bool is_string() const noexcept {
    return _type == type::string;
  }

  bool is_number() const noexcept {
    return _type == type::number;
  }

  bool is_boolean() const noexcept {
    return _type == type::boolean;
  }

  bool is_null() const noexcept {
    return _type == type::null;
  }

  const json_node& operator[](const std::string& key) const {
    if (!is_object()) {
      throw std::runtime_error{"Cannot access object member"};
    }

    auto& object = as_object();

    if (auto itr = object.find(key); itr != object.cend()) {
      return *itr->second;
    } else {
      throw std::runtime_error{"Object does not contain key"};
    }
  }

  json_node& operator[](const std::string& key) {
    return const_cast<json_node&>(std::as_const(*this)[key]);
  }

  const json_node& operator[](const json_node::size_type index) const {
    if (!is_array()) {
      throw std::runtime_error{"Cannot access array member"};
    }

    auto& array = as_array();

    if (index >= array.size()) {
      throw std::runtime_error{"Array does not contain index"};
    }

    return *array[index];
  }

  json_node& operator[](const json_node::size_type index) {
    return const_cast<json_node&>(std::as_const(*this)[index]);
  }

  json_object& as_object() {
    if (_type != type::object) {
      throw std::runtime_error("json_node is not an object");
    }

    return *std::get<std::shared_ptr<json_object>>(_value).get();
  }

  const json_object& as_object() const {
    if (_type != type::object) {
      throw std::runtime_error("json_node is not an object");
    }

    return *std::get<std::shared_ptr<json_object>>(_value).get();
  }

  json_array& as_array() {
    if (_type != type::array) {
      throw std::runtime_error("json_node is not an array");
    }

    return *std::get<std::shared_ptr<json_array>>(_value).get();
  }

  const json_array& as_array() const {
    if (_type != type::array) {
      throw std::runtime_error("json_node is not an array");
    }

    return *std::get<std::shared_ptr<json_array>>(_value).get();
  }

  std::string& as_string() {
    if (_type != type::string) {
      throw std::runtime_error("json_node is not a string");
    }

    return std::get<std::string>(_value);
  }

  const std::string& as_string() const {
    if (_type != type::string) {
      throw std::runtime_error("json_node is not a string");
    }

    return std::get<std::string>(_value);
  }

  sbx::float32& as_number() {
    if (_type != type::number) {
      throw std::runtime_error("json_node is not a number");
    }

    return std::get<sbx::float32>(_value);
  }

  const sbx::float32& as_number() const {
    if (_type != type::number) {
      throw std::runtime_error("json_node is not a number");
    }

    return std::get<sbx::float32>(_value);
  }

  bool& as_boolean() {
    if (_type != type::boolean) {
      throw std::runtime_error("json_node is not a boolean");
    }

    return std::get<bool>(_value);
  }

  const bool& as_boolean() const {
    if (_type != type::boolean) {
      throw std::runtime_error("json_node is not a boolean");
    }

    return std::get<bool>(_value);
  }

private:

  enum class type : sbx::uint8 {
    object,
    array,
    string,
    number,
    boolean,
    null
  };

  std::variant<std::shared_ptr<json_object>, std::shared_ptr<json_array>, std::string, sbx::float32, bool> _value{};
  type _type{};

};

std::ostream& operator<<(std::ostream& os, const json_node& node) {
  switch (node._type) {
    case json_node::type::object: {
      os << "{ ";

      auto first = true;

      for (const auto& [key, value] : node.as_object()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }

        os << "\"" << key << "\": " << *value;
      }

      os << " }";

      break;
    }
    case json_node::type::array: {
      os << "[";

      auto first = true;

      for (const auto& value : node.as_array()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }

        os << *value;
      }

      os << "]";

      break;
    }
    case json_node::type::string: {
      os << "\"" << node.as_string() << "\"";

      break;
    }
    case json_node::type::number: {
      os << node.as_number();

      break;
    }
    case json_node::type::boolean: {
      os << node.as_boolean();

      break;
    }
    case json_node::type::null: {
      os << "null";

      break;
    }
  }

  return os;
}

} // namespace sbx

#endif // SBX_IO_JSON_NODE_HPP_
