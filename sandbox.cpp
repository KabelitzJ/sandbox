#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>

int main(int argc, char** argv) {

  std::cout << "Hello, sandbox!\n";

  std::vector<char*> cli_args(argv, argv + argc);

  return EXIT_SUCCESS;
}