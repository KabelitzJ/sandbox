#ifndef LIBSBX_UTILITY_COMPRESSION_HPP_
#define LIBSBX_UTILITY_COMPRESSION_HPP_

#include <span>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace sbx::utility {

struct compression_error : public std::runtime_error {
  explicit compression_error(std::string_view message)
  : std::runtime_error{std::string{message}} { }
}; // struct compression_error

struct decompression_error : public std::runtime_error {
  explicit decompression_error(std::string_view message)
  : std::runtime_error{std::string{message}} { }
}; // struct decompression_error

enum class compression_type : std::uint8_t {
  lz4 = 0
}; // enum class compression_type

template<compression_type Type>
struct basic_compressor {
  [[nodiscard]] auto compress(std::span<const char> input) -> std::vector<char>;
  [[nodiscard]] auto decompress(std::span<const char> input, const std::size_t original_size) -> std::vector<char>;
}; // struct basic_compressor

template<>
struct basic_compressor<compression_type::lz4> {
  [[nodiscard]] auto compress(std::span<const char> input) -> std::vector<char>;
  [[nodiscard]] auto decompress(std::span<const char> input, const std::size_t original_size) -> std::vector<char>;
}; // struct basic_compressor

using compressor = basic_compressor<compression_type::lz4>;

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_COMPRESSION_HPP_
