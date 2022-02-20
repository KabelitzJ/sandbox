#include <iostream>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <container/container.hpp>
#include <ecs/ecs.hpp>

struct foo { };

int main() {
  auto s = sbx::sparse_set<sbx::uint32>{};

  auto m = sbx::fast_mod<31, 4>();

  std::cout << m << std::endl;

  return 0;
}