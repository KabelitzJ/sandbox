#include <iostream>
#include <cstddef>
#include <type_traits>
#include <bitset>
#include <variant>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <filesystem>

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
#include "json_document.hpp"

struct transform {
  sbx::vector3 position{};
  sbx::vector3 scale{};
  sbx::quaternion rotation{};
};

int main() {

  std::cout << "Hello, Sandbox!" << std::endl;

  auto document = demo::json_document{"demo/assets/config/init.json"};

  const auto& name = document["name"].as_string();

  std::cout << "Name: " << name << std::endl;

  const auto& window = document["window"];

  const auto& resolution = window["resolution"];

  const auto& width = resolution["width"].as_number();
  const auto& height = resolution["height"].as_number();

  std::cout << "Width: " << width << std::endl;
  std::cout << "Height: " << height << std::endl;
  
  return EXIT_SUCCESS;
}