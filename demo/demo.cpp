#include <iostream>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <container/container.hpp>
#include <ecs/ecs.hpp>

struct foo { };

int main() {

  auto foo_container = sbx::component_container<foo>{};

  return 0;
}