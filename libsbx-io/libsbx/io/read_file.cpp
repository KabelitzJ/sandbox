#include <libsbx/io/read_file.hpp>

#include <fstream>

#include <fmt/format.h>

namespace sbx::io {


auto read_file(const std::filesystem::path& path) -> std::vector<std::uint8_t> {
  auto file = std::ifstream{path, std::ios::ate | std::ios::binary};
  auto content = std::vector<std::uint8_t>{};

  if (!file.is_open()) {
    throw std::runtime_error{fmt::format("Failed to open file: {}", path.string())};
  }

  const auto size = static_cast<std::size_t>(file.tellg());

  file.seekg(std::ios::beg);

  content.resize(size);

  file.read(reinterpret_cast<char*>(content.data()), static_cast<std::streamsize>(size));

  file.close();

  return content;
}

} // namespace sbx::io
