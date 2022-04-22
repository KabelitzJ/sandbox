#ifndef DEMO_CONFIGURATION_HPP_
#define DEMO_CONFIGURATION_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include <io/json/json.hpp>

#include "logger.hpp"

namespace demo {

std::vector<std::string> split(const std::string& string, const std::string& delimiter) {
  const auto delimiter_size = delimiter.size();
  auto result = std::vector<std::string>{};

  auto last = std::size_t{0};
  auto next = std::size_t{0};

  while ((next = string.find(delimiter, last)) != std::string::npos) {
    result.push_back(string.substr(last, next - last));
    last = next + delimiter_size;
  }

  result.push_back(string.substr(last));

  return result;
}

class configuration {

public:

  configuration(const std::filesystem::path& path, logger* logger)
  : _document{std::filesystem::absolute(path)},
    _cache{},
    _logger{logger} {}

  ~configuration() = default;

  template<typename Type>
  Type get(const std::string& key) {
    auto node = sbx::json_node{};

    if (const auto itr = _cache.find(key); itr != _cache.cend()) {
      node = itr->second;
    } else {
      const auto path = split(key, ".");

      for (const auto& segment : path) {
        if (node.is_null()) {
          node = _document[segment];
        } else {
          node = node[segment];
        }
      }
      
      _cache.emplace(key, node);
    }

    if constexpr (std::is_same_v<Type, std::string>) {
      return node.as_string();
    } else if (std::is_integral_v<Type> && !std::is_same_v<Type, bool>) {
      const auto copy = sbx::float32{node.as_number()};
      return static_cast<Type>(copy);
    } else if (std::is_floating_point_v<Type>) {
      return static_cast<Type>(node.as_number());
    } else if (std::is_same_v<Type, bool>) {
      return node.as_boolean();
    } else {
      throw std::runtime_error{"Configuration does not support this type"};
    }
  }

private:

  sbx::json_document _document{};
  std::unordered_map<std::string, sbx::json_node> _cache{};
  logger* _logger{};

}; // class configuration

} // namespace demo

#endif // DEMO_CONFIGURATION_HPP_
