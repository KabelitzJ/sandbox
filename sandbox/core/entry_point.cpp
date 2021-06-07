#include "entry_point.hpp"

#include <cstdlib>

#include "core.hpp"

int main(int argc, char** argv) {

  std::vector<std::string> cli_args(argv, argv + argc);

  sbx::setup(cli_args);

  if (!sbx::initialize()) {
    return EXIT_FAILURE;
  }

  sbx::run();

  sbx::terminate();

  return EXIT_SUCCESS;
}
