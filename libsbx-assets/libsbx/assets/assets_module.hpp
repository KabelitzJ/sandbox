#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <libsbx/core/module.hpp>

#include <libsbx/assets/thread_pool.hpp>
#include <libsbx/assets/metadata.hpp>

namespace sbx::assets {

class assets_module : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::post);

public:

  assets_module()
  : _thread_pool{std::thread::hardware_concurrency()} { }

  ~assets_module() override = default;

  auto update() -> void override {

  }

  template<typename Function, typename... Args>
  requires (std::is_invocable_v<Function, Args...>)
  auto submit(Function&& function, Args&&... args) -> std::future<std::invoke_result_t<Function, Args...>> {
    return _thread_pool.submit(std::forward<Function>(function), std::forward<Args>(args)...);
  }

private:

  thread_pool _thread_pool;

  

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
