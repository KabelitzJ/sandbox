#ifndef DEMO_CONFIGURATION_HPP_
#define DEMO_CONFIGURATION_HPP_

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>

#include <utils/noncopyable.hpp>

#include <types/primitives.hpp>

#include <math/vector2.hpp>

#include "hashed_string.hpp"

namespace demo {

struct version {
  sbx::uint32 major{};
  sbx::uint32 minor{};
  sbx::uint32 patch{};
}; // struct version

class configuration : sbx::noncopyable {

public:

  configuration(const std::filesystem::path& path)
  : _name{},
    _version{},
    _window_resolution{},
    _is_fullscreen{} {
    _initialize(path);
  }

  ~configuration() = default;

  const std::string& app_name() const noexcept {
    return _name;
  }

  const version& app_version() const noexcept {
    return _version;
  }

  const sbx::vector2i& window_resolution() const noexcept {
    return _window_resolution;
  }

  bool is_fullscreen() const noexcept {
    return _is_fullscreen;
  }

private:

  std::vector<std::string> _split(const std::string& string, const std::string& delimiter) {
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

  void _initialize(const std::filesystem::path& path) {
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

    auto config = nlohmann::json{};

    file >> config;

    file.close();

    if (config.empty()) {
      throw std::runtime_error{"Configuration file is empty"};
    }

    const auto& app = config.at("app");

    app.at("name").get_to(_name);

    const auto& version = app.at("version");

    version.at("major").get_to(_version.major);
    version.at("minor").get_to(_version.minor);
    version.at("patch").get_to(_version.patch);

    const auto& window = config.at("window");

    const auto& resolution = window.at("resolution");

    resolution.at("width").get_to(_window_resolution.x);
    resolution.at("height").get_to(_window_resolution.y);

    window.at("is_fullscreen").get_to(_is_fullscreen);
  }

  std::string _name{};
  version _version{};
  sbx::vector2i _window_resolution{};
  bool _is_fullscreen{};

}; // class configuration

} // namespace demo

#endif // DEMO_CONFIGURATION_HPP_
