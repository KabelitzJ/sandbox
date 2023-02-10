#ifndef LIBSBX_IO_READ_FILE_HPP_
#define LIBSBX_IO_READ_FILE_HPP_

#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <fmt/format.h>

namespace sbx::io {

auto read_file(const std::filesystem::path& path) -> std::string {
  auto file = std::ifstream{path};
  auto content = std::string{};

  if (!file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open file: {}", path.string())};
  }

  auto string = std::stringstream{};

  string << file.rdbuf();

  file.close();

  return string.str();
}

auto read_file_as_lines(const std::filesystem::path& path) -> std::vector<std::string> {
  auto file = std::ifstream{path};
  auto lines = std::vector<std::string>{};

  if (file.is_open()) {
    auto line = std::string{};
    while (std::getline(file, line)) {
      lines.push_back(line);
    }
    file.close();
  }

  return lines;
}

} // namespace sbx::io

#endif // LIBSBX_IO_READ_FILE_HPP_
