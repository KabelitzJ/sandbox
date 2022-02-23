#include <iostream>
#include <cstddef>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <container/container.hpp>
#include <ecs/ecs.hpp>

struct foo {
  sbx::uint32 i{};
};

int main() {

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;

  auto m4 = sbx::matrix4x4::zero;

  auto q = sbx::quaternion{};

  auto set = sbx::sparse_set<sbx::uint32>{};

  auto container = sbx::component_container<sbx::uint32, foo>{};

  container.emplace(0, sbx::uint32{32});
  container.emplace(1, sbx::uint32{52});
  container.emplace(2, sbx::uint32{72});
  container.emplace(3, sbx::uint32{92});
  container.emplace(4, sbx::uint32{112});
  container.emplace(5, sbx::uint32{132});
  container.emplace(6, sbx::uint32{152});


  for (const auto& component : container) {
    std::cout << component.i << std::endl;
  }

  std::cout << std::endl;

  container.remove(0);
  container.remove(1);
  container.remove(2);
  container.remove(3);

  for (const auto& component : container) {
    std::cout << component.i << std::endl;
  }

  std::cout << std::endl;

  std::cout << std::boolalpha;

  std::cout << "0: " << container.contains(0) << std::endl;
  std::cout << "1: " << container.contains(1) << std::endl;
  std::cout << "2: " << container.contains(2) << std::endl;
  std::cout << "3: " << container.contains(3) << std::endl;
  std::cout << "4: " << container.contains(4) << std::endl;
  std::cout << "5: " << container.contains(5) << std::endl;
  std::cout << "6: " << container.contains(6) << std::endl;
  std::cout << "7: " << container.contains(7) << std::endl;

  std::cout << std::noboolalpha;

  auto r = sbx::registry{};

  auto& f = r.add_component<foo>(0, sbx::uint32{32});

  f.i = 42;
    
  return EXIT_SUCCESS;
}