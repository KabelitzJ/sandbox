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
#include <utils/utils.hpp>
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

  auto p = demo::json_parser{"demo/assets/config/init.json"};

  p.parse();

  // std::cout << *p.root() << std::endl;

  auto first_name = p.root()->as_object()["name"]->as_object()["first"]->as_string();

  std::cout << first_name << std::endl;
  
  return EXIT_SUCCESS;
}