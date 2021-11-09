#ifndef SBX_UTILS_FILE_READER_HPP_
#define SBX_UTILS_FILE_READER_HPP_

#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <limits>

#include <types/primitives.hpp>

namespace sbx {

std::string read_file(const std::string& path) {
  // [TODO] KAJ 2021-11-09 22:53 - Search for a faster way to read a file. Maybe libc?

  auto buffer = std::stringstream{};

  auto file = std::ifstream{path, std::ios::in | std::ios::binary};

  assert(file.is_open()); // File could not be opened

  buffer << file.rdbuf();

  file.close();

  return buffer.str();
}

} // namespace sbx

#endif // SBX_UTILS_FILE_READER_HPP_
