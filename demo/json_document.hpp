#ifndef DEMO_JSON_DOCUMENT_HPP_
#define DEMO_JSON_DOCUMENT_HPP_

#include <filesystem>
#include <memory>

#include "json_node.hpp"

namespace demo {

class json_document {

public:

  json_document(const std::filesystem::path& path)
  : _root{demo::json_parser{path}.parse()} {}

  ~json_document() = default;

  const demo::json_node& operator[](const std::string& key) const {
    if (!_root->is_object()) {
      throw std::runtime_error{"Cannot access object member"};
    }

    auto& object = _root->as_object();

    if (auto itr = object.find(key); itr != object.cend()) {
      return *itr->second;
    } else {
      throw std::runtime_error{"Object does not contain key"};
    }
  }

  demo::json_node& operator[](const std::string& key) {
    return const_cast<demo::json_node&>(std::as_const(*this)[key]);
  }

  const demo::json_node& operator[](const demo::json_node::size_type index) const {
    if (!_root->is_array()) {
      throw std::runtime_error{"Cannot access array member"};
    }

    auto& array = _root->as_array();

    if (index >= array.size()) {
      throw std::runtime_error{"Array does not contain index"};
    }

    return *array[index];
  }

  demo::json_node& operator[](const demo::json_node::size_type index) {
    return const_cast<demo::json_node&>(std::as_const(*this)[index]);
  }

private:

  std::shared_ptr<demo::json_node> _root{};

};

} // namespace demo

#endif // DEMO_JSON_DOCUMENT_HPP_
