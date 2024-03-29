#include <libsbx/core/core.hpp>

#include <demo/demo_application.hpp>

auto sbx::core::create_application() -> std::unique_ptr<sbx::core::application> {
  return std::make_unique<demo::demo_application>();
}
