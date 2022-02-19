#include <iostream>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <container/container.hpp>
#include <ecs/ecs.hpp>

int main() {

  auto vector = sbx::vector3{1.0f, 2.0f, 3.0f};
  auto matrix = sbx::matrix4x4::identity;

  auto result = matrix * sbx::vector4{vector};

  std::cout << result << std::endl;

  using namespace sbx::literals;

  const auto q = sbx::quaternion{sbx::vector3::up, 90.0_degrees};

  auto id = sbx::type_id<sbx::vector3>{};

  std::cout << "vector3 id: " << sbx::uint32{id} << std::endl;

  auto set = sbx::sparse_set<sbx::uint32>{};

  set.insert(sbx::uint32{1});
  set.insert(sbx::uint32{2});
  set.insert(sbx::uint32{3});
  set.insert(sbx::uint32{3});
  set.insert(sbx::uint32{51});
  set.insert(sbx::uint32{22});

  for (const auto& value : set) {
    std::cout << value << std::endl;
  }

  std::cout << std::endl;

  set.remove(sbx::uint32{3});
  set.remove(sbx::uint32{54});

  for (const auto& value : set) {
    std::cout << value << std::endl;
  }

  return 0;
}