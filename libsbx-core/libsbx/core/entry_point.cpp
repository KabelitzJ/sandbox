#include <libsbx/core/entry_point.hpp>

#include <span>
#include <ranges>

#include <range/v3/all.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/core/exit.hpp>
#include <libsbx/core/logger.hpp>

auto main(int argc, const char** argv) -> int {
  auto args = std::vector<std::string_view>{argv, argv + argc};

  try {
    auto engine = std::make_unique<sbx::core::engine>(args);

    auto application = sbx::core::create_application();

    engine->run(std::move(application));
  } catch(const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return sbx::core::exit::failure; 
  }

  return sbx::core::exit::success;
}
