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
  // auto app = demo::application{"demo/config/app.json"};

  // return app.start();

  auto doc = sbx::json_document{"demo/config/app.json"};

  std::cout << doc.dump();

  return 0;
}
