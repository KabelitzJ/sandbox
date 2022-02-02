#include <iostream>

#include <math/math.hpp>

int main() {

  auto vector1 = sbx::vector3{1.0f, 2.0f, 3.0f};
  auto vector2 = sbx::vector3{3.0f, 2.0f, 1.0f};

  auto vector3 = vector1 + vector2 * 2.0f + sbx::vector3::forward;

  vector3.normalize();

  std::cout << vector3 << std::endl;
  std::cout << vector3.length() << std::endl;

  auto m = sbx::matrix4x4::identity();

  m[1][3] = 2.0f;

  std::cout << m << std::endl;

  return 0;
}
