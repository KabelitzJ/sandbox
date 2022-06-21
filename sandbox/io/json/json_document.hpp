#ifndef SBX_IO_JSON_DOCUMENT_HPP_
#define SBX_IO_JSON_DOCUMENT_HPP_

#include <filesystem>
#include <memory>

#include "json_node.hpp"

namespace sbx {

class json_document {

public:

  json_document()
  : _root{nullptr} {}

  json_document(const std::filesystem::path& path)
  : _root{json_parser{path}.parse()} {}

  ~json_document() = default;

  const json_node& operator[](const std::string& key) const {
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

  json_node& operator[](const std::string& key) {
    return const_cast<json_node&>(std::as_const(*this)[key]);
  }

  const json_node& operator[](const json_node::size_type index) const {
    if (!_root->is_array()) {
      throw std::runtime_error{"Cannot access array member"};
    }

    auto& array = _root->as_array();

    if (index >= array.size()) {
      throw std::runtime_error{"Array does not contain index"};
    }

    return *array[index];
  }

  json_node& operator[](const json_node::size_type index) {
    return const_cast<json_node&>(std::as_const(*this)[index]);
  }

  std::string dump(const size_t indent = 0) const {
    auto stream = std::stringstream{};

    stream << *_root;

    return stream.str();
  }

private:

  std::shared_ptr<json_node> _root{};

};

} // namespace sbx

#endif // SBX_IO_JSON_DOCUMENT_HPP_
