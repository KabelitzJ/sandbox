#include "entry.hpp"

#include <cstdlib>
#include <vector>
#include <string>

#include <engine/engine.hpp>

int main(int argc, char** argv) {
  const auto cli_args = std::vector<std::string>{argv, argv + argc};
  
  sbx::entry();

  auto engine = sbx::engine{};

  engine.start();

  return EXIT_SUCCESS;
}