#include "entry_point.hpp"

#include <cstdlib>

#include "engine.hpp"
#include "module.hpp"

int main(int argc, char** argv) {

  const auto cli_args = std::vector<std::string_view>{argv, argv + argc};

  auto engine = sbx::engine{};

  sbx::setup(engine);

  engine.initialize();

  engine.start();

  return EXIT_SUCCESS;
}
