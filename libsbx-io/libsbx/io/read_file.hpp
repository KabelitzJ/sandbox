#ifndef LIBSBX_IO_READ_FILE_HPP_
#define LIBSBX_IO_READ_FILE_HPP_

#include <filesystem>
#include <vector>

namespace sbx::io {

auto read_file(const std::filesystem::path& path) -> std::vector<char>;

} // namespace sbx::io

#endif // LIBSBX_IO_READ_FILE_HPP_
