#include <iostream>
#include <vector>
#include <cstdlib>

#include <core/core.hpp>

int main(int argc, char** argv) {

  std::vector<char*> cli_args(argv, argv + argc);

  std::cout << "Hello, demo!\n";

  std::cout << "31 + 44 = " << sbx::add(31, 44) << "\n";

  return EXIT_SUCCESS;
}
