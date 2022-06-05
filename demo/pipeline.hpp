#ifndef DEMO_PIPELINE_HPP_
#define DEMO_PIPELINE_HPP_

#include <filesystem>
#include <unordered_set>
#include <string>

#include <io/file_reader.hpp>

#include "logger.hpp"

namespace demo {

class pipeline {

public:

  pipeline(const std::filesystem::path& path) {
    const auto absolute_path = std::filesystem::absolute(path);
    _initialize_shaders(absolute_path);
  }

  ~pipeline() = default;

private:

  void _initialize_shaders(const std::filesystem::path& path) {
    if (!std::filesystem::is_directory(path)) {
      throw std::runtime_error{"Shader does not exist: " + path.string()};
    }

    const auto shader_binary_path = path / "bin";

    if (!std::filesystem::exists(shader_binary_path)) {
      throw std::runtime_error{"Shader has not been compiled: " + path.string()};
    }

    for (const auto& entry : std::filesystem::directory_iterator{shader_binary_path}) {
      if (!entry.is_regular_file()) {
        continue;
      }

      if (!entry.path().has_extension() || entry.path().extension() != ".spv") {
        continue;
      }
      
      // const auto shader_source = sbx::get_file_contents(entry.path());
    }
  }

}; // class pipeline

} // namespace demo

#endif // DEMO_PIPELINE_HPP_
