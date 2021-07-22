#include "entry_point.hpp"

#include <cstdlib>

#include "core.hpp"

int main(int argc, char** argv) {

  const auto cli_args = std::vector<std::string_view>{argv, argv + argc};

  sbx::setup(cli_args);

  return EXIT_SUCCESS;
}
