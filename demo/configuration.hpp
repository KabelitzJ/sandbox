#ifndef DEMO_CONFIGURATION_HPP_
#define DEMO_CONFIGURATION_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <utils/noncopyable.hpp>

#include "hashed_string.hpp"
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

class configuration : sbx::noncopyable {

public:

  configuration(const std::filesystem::path& path)
  : _json{},
    _cache{} {
    _load(path);
  }

  ~configuration() = default;

  template<typename Type>
  Type get(const std::string& key) {
    if (auto it = _cache.find(key); it != _cache.end()) {
      return it->second.get<Type>();
    }

    const auto path = split(key, ".");

    auto json = _json;
 
    for (const auto& part : path) {
      if (!json.contains(part)) {
        throw std::runtime_error{"Key not found in configuration: " + key};
      }

      json = json[part];
    }

    _cache[key] = json;

    return json.get<Type>();
  }

  void dump() {
    std::cout << _json.dump(4) << std::endl;
  }

private:

  void _load(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      throw std::runtime_error{"Configuration file does not exist"};
    }

    if (!std::filesystem::is_regular_file(path) || !path.has_extension() || path.extension() != ".json") {
      throw std::runtime_error{"Configuration file is not a valid file"};
    }

    auto file = std::ifstream{path};
    
    if (!file.is_open()) {
      throw std::runtime_error{"Could not open configuration file"};
    }

    file >> _json;

    if (_json.empty()) {
      throw std::runtime_error{"Configuration file is empty"};
    }

    file.close();
  }

  nlohmann::json _json{};
  std::unordered_map<hashed_string, nlohmann::json> _cache{};


}; // class configuration

} // namespace demo

#endif // DEMO_CONFIGURATION_HPP_
