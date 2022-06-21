#ifndef DEMO_CONFIGURATION_HPP_
#define DEMO_CONFIGURATION_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann_json/json.hpp>

#include <utils/noncopyable.hpp>

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

  configuration(const std::filesystem::path& path) { }

  ~configuration() = default;

  template<typename Type>
  Type get(const std::string& key) {
    return Type{};
  }

private:




}; // class configuration

} // namespace demo

#endif // DEMO_CONFIGURATION_HPP_
