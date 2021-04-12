#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>

#include <thread_pool.hpp>
#include <task_graph.hpp>

class foo {

public:
  foo(int data) : _data(data) {
    std::cout << "created(" << _data << ")" << std::endl;
  }

  ~foo() {
    std::cout << "deleted(" << _data << ")" << std::endl;
  }

private:
  int _data;

};

int main(int argc, char** argv) {

  std::cout << "Hello, sandbox!\n";

  std::vector<char*> cli_args(argv, argv + argc);

  tp::thread_pool pool;

  auto result = pool.enqueue([](int data){ return data; }, 12);

  std::cout << "result: " << result.get() << '\n';

  tg::task_graph graph;
  tg::object_pool<foo> object_pool(2);

  auto object1 = object_pool.request(12);
  auto object2 = object_pool.request(2);
  auto object3 = object_pool.request(31);

  return EXIT_SUCCESS;
}