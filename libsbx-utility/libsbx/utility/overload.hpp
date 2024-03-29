#ifndef LIBSBX_UTILITY_OVERLOAD_HPP_
#define LIBSBX_UTILITY_OVERLOAD_HPP_

namespace sbx::utility {

template<typename... Types>
struct overload : Types... {
  using Types::operator()...;
}; // struct overload

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_OVERLOAD_HPP_
