#include <iostream>

#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <math/vector4.hpp>

int main() {

  auto vector1 = sbx::vector3{1.0f, 2.0f, 3.0f};
  auto vector2 = sbx::vector3{3.0f, 2.0f, 1.0f};

  auto vector3 = vector1 + vector2 * 2.0f + sbx::vector3::forward;

  vector3.normalize();

  auto v4 = sbx::vector4{1.0f, 2.0f, 3.0f, 4.0f};

  auto v5 = sbx::vector4::normalized(v4);

  std::cout << vector3 << std::endl;
  std::cout << vector3.length() << std::endl;

  return 0;
}
