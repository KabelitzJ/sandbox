#ifndef SBX_UTILS_FILE_READER_HPP_
#define SBX_UTILS_FILE_READER_HPP_

#include <string>
#include <sstream>
#include <fstream>
#include <exception>

namespace sbx {

std::string read_file(const std::string& path) {
  auto file = std::ifstream{path};

  if (!file.is_open()) {
    auto error = std::stringstream{};
    error << "[Error] Failed to open file: " << path;

    throw std::runtime_error{error.str()};
  }

  auto buffer = std::stringstream{};

  buffer << file.rdbuf();

  return buffer.str();
}

} // namespace sbx

#endif // SBX_UTILS_FILE_READER_HPP_
