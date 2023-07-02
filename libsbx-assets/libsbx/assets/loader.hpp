#ifndef LIBSBX_ASSETS_LOADER_HPP_
#define LIBSBX_ASSETS_LOADER_HPP_

#include <filesystem>
#include <memory>

namespace sbx::assets {

template<typename Type>
struct loader {
  auto operator()(const std::filesystem::path& path) const noexcept -> std::unique_ptr<Type>;
}; // struct loader

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_LOADER_HPP_
