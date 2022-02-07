#include <cassert>

namespace sbx {

template<typename Type>
inline constexpr typename basic_quaternion<Type>::reference basic_quaternion<Type>::operator[](index_type index) noexcept {
  assert(index < 4);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
    case 2: {
      return z;
    }
    case 3: {
      return w;
    }
  }
}

template<typename Type>
inline constexpr typename basic_quaternion<Type>::const_reference basic_quaternion<Type>::operator[](index_type index) const noexcept {
  assert(index < 4);

  switch (index) {
    default:
    case 0: {
      return x;
    }
    case 1: {
      return y;
    }
    case 2: {
      return z;
    }
    case 3: {
      return w;
    }
  }
}

template<typename Type>
inline constexpr typename basic_quaternion<Type>::pointer basic_quaternion<Type>::data() noexcept {
  return &x;
}

template<typename Type>
inline constexpr typename basic_quaternion<Type>::const_pointer basic_quaternion<Type>::data() const noexcept {
  return &x;
}


} // namespace sbx
