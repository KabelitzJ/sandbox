#ifndef LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_

#include <vulkan/vulkan.hpp>

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>
#include <libsbx/utility/target.hpp>

#include <libsbx/memory/blob.hpp>

namespace sbx::graphics {

class compiler {

public:

  struct define {
    std::string key;
    std::string value;
  }; // struct define

  struct compile_request {
    std::filesystem::path path;
    SlangStage stage;
    std::string entry_point{"main"};
    std::vector<define> defines;
    std::vector<std::filesystem::path> includes;
  }; // struct compile_request

  compiler();

  ~compiler();

  auto compile(const compile_request& compile_request) -> std::vector<std::uint32_t>;

private:

  static auto _read_file(const std::filesystem::path& path) -> std::string;

  auto _initialize_session() -> void;

  Slang::ComPtr<slang::IGlobalSession> _global_session;
  Slang::ComPtr<slang::ISession> _session;

}; // class compiler

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_
