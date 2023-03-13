#ifndef LIBSBX_ASYNC_ASYNC_MODULE_HPP_
#define LIBSBX_ASYNC_ASYNC_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/async/loader.hpp>

namespace sbx::async {

class async_module : public core::module<async_module> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  async_module()
  : _loader{std::make_unique<async::loader>()} {

  }

  ~async_module() override {

  }

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {

  }

  auto loader() -> async::loader& {
    return *_loader;
  }  

private:

  std::unique_ptr<async::loader> _loader{};

}; // class async_module

} // namespace sbx::async

#endif // LIBSBX_ASYNC_ASYNC_MODULE_HPP_
