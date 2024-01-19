#ifndef LIBSBX_IO_LOADER_FACTORY_HPP_
#define LIBSBX_IO_LOADER_FACTORY_HPP_

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

namespace sbx::io {

template<typename Type, typename Result>
class loader_factory {

public:

  template<typename Derived>
  requires (std::is_invocable_r_v<Result, decltype(Derived::load), const std::filesystem::path&>)
  class loader {

  public:

    using result_type = Result;

  protected:

    template<typename... Extensions>
    static auto register_extensions(Extensions&&... extensions) -> bool {
      ((loader_factory<Type, Result>::_loaders()[extensions] = &Derived::load), ...);

      return true;
    }

  }; // struct loader

protected:

  using loader_container_type = std::unordered_map<std::string, std::function<Result(const std::filesystem::path&)>>;

  static auto _loaders() -> loader_container_type& {
    static auto loaders = loader_container_type{};
    return loaders;
  }

}; // class loader_factory

} // namespace sbx::io

#endif // LIBSBX_IO_LOADER_FACTORY_HPP_
