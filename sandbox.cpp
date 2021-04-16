#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>

#include <thread_pool.hpp>
#include <object_pool.hpp>
#include <resource_pool.hpp>

class foo {

public:
  foo(int data) : _data(data) {
    std::cout << "created(" << _data << ")" << std::endl;
  }

  ~foo() {
    std::cout << "deleted(" << _data << ")" << std::endl;
  }

  int data() const {
    return _data;
  }

private:
  int _data;

};

int main(int argc, char** argv) {

  std::cout << "Hello, sandbox!\n";

  std::vector<char*> cli_args(argv, argv + argc);

  sbx::thread_pool pool;

  auto result = pool.enqueue([](int data){ return data; }, 12);

  std::cout << "result: " << result.get() << '\n';

  sbx::object_pool<foo> object_pool(2);

  sbx::resource_pool<foo> rpool(64);

  sbx::resource_pool<foo>::handle item1 = rpool.acquire_handle(13);
  sbx::resource_pool<foo>::pointer item2 = rpool.create(33);

  std::cout << item1->data() << std::endl;

  rpool.destroy(item2);

  return EXIT_SUCCESS;
}