#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>

#include <thread_pool.hpp>
#include <task_graph.hpp>

int main(int argc, char** argv) {

  std::cout << "Hello, sandbox!\n";

  std::vector<char*> cli_args(argv, argv + argc);

  tp::thread_pool pool;
  tg::task_graph graph;

  auto result = pool.enqueue([](int data){ return data; }, 12);

  std::cout << "result: " << result.get() << '\n';

  tg::uuid id;

  std::cout << id << std::endl;

  return EXIT_SUCCESS;
}