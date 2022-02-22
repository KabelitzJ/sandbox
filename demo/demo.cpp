#include <iostream>
#include <cstddef>

#include <utils/utils.hpp>
#include <math/math.hpp>
#include <container/container.hpp>
#include <ecs/ecs.hpp>

struct foo { };

int main() {

  auto v2 = sbx::vector2::zero;
  auto v3 = sbx::vector3::zero;
  auto v4 = sbx::vector4::zero;

  auto m4 = sbx::matrix4x4::zero;

  auto q = sbx::quaternion{};

  auto set = sbx::sparse_set<sbx::uint32>{};
      
  set.insert(3);
  set.insert(5);
  set.insert(1);
  set.insert(2);
  set.insert(9);
  set.insert(4);
  set.insert(0);
  set.insert(6);
  set.insert(8);
  set.insert(7);  

  std::cout << "set.size() = " << set.size() << std::endl;

  set.erase(3);
  set.erase(5);
  set.erase(1);
  set.erase(1);
  set.erase(12); 
    
  return EXIT_SUCCESS;
}