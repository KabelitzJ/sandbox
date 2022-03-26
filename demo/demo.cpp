#include <iostream>
#include <cstddef>
#include <type_traits>
#include <bitset>
#include <variant>

#include <math/math.hpp>
#include <types/types.hpp>
#include <container/container.hpp>
#include <memory/memory.hpp>
#include <ecs/ecs.hpp>

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

  auto root = p.root();
  
  return EXIT_SUCCESS;
}