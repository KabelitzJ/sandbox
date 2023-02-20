#ifndef LIBSBX_IO_FILE_HPP_
#define LIBSBX_IO_FILE_HPP_

#include <filesystem>
#include <memory>
#include <utility>

template<typename Type>
concept file_format = requires {
  std::negation_v<std::is_abstract<Type>>;
  std::is_constructible_v<Type, const std::filesystem::path&>;
};

template<file_format Format>
class basic_file : public Format {

public:

  using format_type = Format;

  basic_file(const std::filesystem::path& path)
  : format_type{path} { }

  ~basic_file() {
    // // [NOTE] KAJ 2023-02-20 17:24 - We assume that the file format may hold resources that need to be released.
    std::destroy_at<format_type>(this);
  }

private:

}; // class basic_file_format

#endif // LIBSBX_IO_FILE_HPP_
