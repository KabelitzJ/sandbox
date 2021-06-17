#include "entry_point.hpp"

#include <cstdlib>

#include <core/core.hpp>

int main(int argc, char** argv) {

  std::vector<std::string> cli_args(argv, argv + argc);

  sbx::setup(cli_args);

  sbx::engine engine;

  engine.start();

  return EXIT_SUCCESS;
}
