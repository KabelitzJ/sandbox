#include "file_utils.hpp"

#include <fstream>
#include <exception>
#include <sstream>
#include <filesystem>

namespace sbx {

std::string read_file_contents(const std::filesystem::path& file_path) {
  std::ifstream ifs(file_path, std::ios::binary);

  if (!ifs.is_open()) {
    std::stringstream ss;

    ss << "Could not open file: '" << file_path << "'!\n";

    throw std::runtime_error(ss.str());
  }

  std::string file_contents;

  ifs.seekg(0, std::ios::end);

  std::uintmax_t size = std::filesystem::file_size(file_path);
  file_contents.resize(size);

  ifs.seekg(0, std::ios::beg);
  ifs.read(file_contents.data(), file_contents.size());
  ifs.close();

  return file_contents;
}

} // namespace sbx
