#ifndef SBX_CORE_FILE_UTILS_HPP_
#define SBX_CORE_FILE_UTILS_HPP_

#include <string>
#include <filesystem>

namespace sbx {

std::string read_file_contents(const std::filesystem::path& file_path);

} // namespace sbx

#endif // SBX_CORE_FILE_UTILS_HPP_
