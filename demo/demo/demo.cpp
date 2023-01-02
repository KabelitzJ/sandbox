#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>
#include <libsbx/devices/devices.hpp>

auto main(int argc, char** argv) -> int {
  sbx::core::logger::info("Hello, World!");

  try {
    auto engine = std::make_unique<sbx::core::engine>(std::vector<std::string_view>{argv, argv + argc});
    engine->run();
  } catch(const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
