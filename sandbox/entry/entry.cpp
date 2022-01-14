#include "entry.hpp"

#include <cstdlib>
#include <vector>
#include <string>

int main(int argc, char** argv) {
  const auto cli_args = std::vector<std::string>{argv, argv + argc};
  
  sbx::entry();

  return EXIT_SUCCESS;
}