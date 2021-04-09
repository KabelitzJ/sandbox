#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>

#include "thread_pool.hpp"

int main(int argc, char** argv) {

  std::cout << "Hello, sandbox!\n";

  std::vector<char*> cli_args(argv, argv + argc);

  thread_pool pool;

  auto result = pool.enqueue([](int data){ return data; }, 12);

  std::cout << "result: " << result.get() << '\n';

  return EXIT_SUCCESS;
}