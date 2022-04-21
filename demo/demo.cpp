#include <iostream>

#include <math/math.hpp>
#include <types/types.hpp>
#include <container/container.hpp>
#include <memory/memory.hpp>
#include <ecs/ecs.hpp>
#include <utils/utils.hpp>
#include <async/async.hpp>
#include <io/io.hpp>
#include <core/core.hpp>

#include "application.hpp"

int main() {
  auto document = sbx::json_document{"demo/config/app.json"};

  const auto& name = document["name"].as_string();

  auto app = demo::application{name};

  try {
    app.run();
  } catch (const std::exception& exception) {
    std::cout << exception.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}