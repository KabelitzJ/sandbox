#ifndef LIBSBX_ASSETS_ASSETS_MODULE_HPP_
#define LIBSBX_ASSETS_ASSETS_MODULE_HPP_

#include <libsbx/core/module.hpp>

namespace sbx::assets {

class assets_module final : public core::module<assets_module> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  assets_module() = default;

  ~assets_module() override = default;

  auto update() -> void override {

  }

private:

}; // class assets_module

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_ASSETS_MODULE_HPP_
