#include <iostream>

#include <math/math.hpp>

int main() {

  auto vector = sbx::vector3{1.0f, 2.0f, 3.0f};
  auto matrix = sbx::matrix4x4::identity;

  auto result = matrix * sbx::vector4{vector};

  std::cout << result << std::endl;

  using namespace sbx::literals;

  auto angle = sbx::angle{45.0_degrees};

  std::cout << angle.to_degrees() << std::endl;
  std::cout << angle.to_radians() << std::endl;

  auto angle2 = sbx::angle{90.0_degrees};

  auto equal = angle == angle2;

  std::cout << "equal: " << equal << std::endl;

  return 0;
}
