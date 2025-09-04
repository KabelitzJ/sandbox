#include <libsbx/core/entry_point.hpp>

#include <easy/profiler.h>

#include <span>
#include <ranges>
#include <stacktrace>

#include <range/v3/all.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/core/exit.hpp>

auto main(int argc, const char** argv) -> int {
  EASY_PROFILER_ENABLE;
  EASY_MAIN_THREAD;
  profiler::startListen();

  auto args = std::vector<std::string_view>{argv, argv + argc};

  EASY_BLOCK("main");

  try {
    auto engine = std::make_unique<sbx::core::engine>(args);

    auto application = sbx::core::create_application();

    engine->run(std::move(application));
  } catch(const std::exception& exception) {
    sbx::utility::logger<"core">::error("{}\n{}", exception.what(), std::to_string(std::stacktrace::current()));
    return sbx::core::exit::failure; 
  }

  EASY_END_BLOCK;

  profiler::stopListen();

  profiler::dumpBlocksToFile("libsbx.profile");

  return sbx::core::exit::success;
}
