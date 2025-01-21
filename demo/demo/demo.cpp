#include <libsbx/core/core.hpp>

#include <demo/application.hpp>

auto sbx::core::create_application() -> std::unique_ptr<sbx::core::application> {
  return std::make_unique<demo::application>();
}
