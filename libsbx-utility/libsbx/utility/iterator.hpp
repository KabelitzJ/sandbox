#ifndef LIBSBX_UTILITY_ITERATOR_HPP_
#define LIBSBX_UTILITY_ITERATOR_HPP_

#include <cstddef>

namespace sbx::utility {

template<typename Category, typename Type, typename Distance = std::ptrdiff_t, typename Pointer = Type*, typename Reference = Type&>
struct iterator {
  using iterator_category = Category;
  using value_type = Type;
  using difference_type = Distance;
  using pointer = Pointer;
  using reference = Reference;
}; // struct iterator

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_ITERATOR_HPP_
