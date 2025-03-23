#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <libsbx/core/module.hpp>

#include <libsbx/assets/thread_pool.hpp>
#include <libsbx/assets/metadata.hpp>

namespace sbx::assets {

class assets_module : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::post);

public:

  assets_module() = default;

  ~assets_module() override = default;

  auto update() -> void override {

  }

private:

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
