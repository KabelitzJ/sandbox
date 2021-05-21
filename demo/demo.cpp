#include <iostream>
#include <vector>
#include <cstdlib>

#include <core/core.hpp>
#include <evtsys/event_queue.hpp>
#include <evtsys/event.hpp>

int main(int argc, char** argv) {

  std::vector<char*> cli_args(argv, argv + argc);

  std::cout << "Hello, demo!\n";

  if (!sbx::initialize()) {
    return EXIT_FAILURE;
  }

  sbx::run();
  
  sbx::terminate();

  return EXIT_SUCCESS;
}
