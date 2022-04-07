#include <iostream>
#include <cstddef>
#include <type_traits>
#include <bitset>
#include <variant>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

#include <math/math.hpp>
#include <types/types.hpp>
#include <container/container.hpp>
#include <memory/memory.hpp>
#include <ecs/ecs.hpp>
#include <async/async.hpp>

#include "json_node.hpp"
#include "json_tokenizer.hpp"
#include "json_parser.hpp"

struct transform {
  sbx::vector3 position{};
  sbx::vector3 scale{};
  sbx::quaternion rotation{};
};

int main() {

  std::cout << "Hello, Sandbox!" << std::endl;

  // auto p = demo::json_parser{"demo/assets/config/init.json"};

  // p.parse();

  // std::cout << *p.root() << std::endl;

  // auto values = p.root()->as_array();

  // auto c = sbx::float32{0};

  // for (const auto& value : values) {
  //   c += value->as_number();
  // }

  // std::cout << c << std::endl;

  auto p = sbx::thread_pool{};

  auto f1 = p.enqueue([](){
    return 42;
  });

  std::cout << f1.get() << std::endl;
  
  return EXIT_SUCCESS;
}