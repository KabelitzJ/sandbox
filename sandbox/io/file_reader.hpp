#ifndef SBX_IO_FILE_READER_HPP_
#define SBX_IO_FILE_READER_HPP_

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <vector>

#include <types/primitives.hpp>

namespace sbx {

std::string get_file_contents(const std::filesystem::path& path, std::ios::openmode mode = std::ios::in) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error{"File does not exist: " + path.string()};
  }

  const auto absolute_path = std::filesystem::absolute(path);

  if (!std::filesystem::is_regular_file(absolute_path)) {
    throw std::runtime_error{"File is not a regular file: " + absolute_path.string()};
  }

  auto file_handle = std::ifstream{absolute_path, mode};
  file_handle.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  if (!file_handle.is_open()) {
    throw std::runtime_error{"Failed to open file: " + absolute_path.string()};
  }

  auto buffer = std::stringstream{};

  buffer << file_handle.rdbuf();

  file_handle.close();

  return buffer.str();
}

} // namespace sbx

#endif // SBX_IO_FILE_READER_HPP_
