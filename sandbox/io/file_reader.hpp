#ifndef SBX_IO_FILE_READER_HPP_
#define SBX_IO_FILE_READER_HPP_

#include <filesystem>
#include <fstream>
#include <vector>
#include <iterator>

#include <types/primitives.hpp>

namespace sbx {

std::vector<char> get_file_contents(const std::filesystem::path& path) {
  auto file_handle = std::ifstream{path, std::ios::binary};

  if (!file_handle.is_open()) {
    throw std::runtime_error{"Failed to open file: " + path.string()};
  }

  auto buffer = std::vector<char>{};

  buffer.insert(buffer.end(), std::istreambuf_iterator<char>{file_handle}, std::istreambuf_iterator<char>{});

  return buffer;
}

} // namespace sbx

#endif // SBX_IO_FILE_READER_HPP_
