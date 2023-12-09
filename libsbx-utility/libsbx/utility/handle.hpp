#ifndef LIBSBX_UTILITY_HANDLE_HPP_
#define LIBSBX_UTILITY_HANDLE_HPP_

#include <cstdint>
#include <limits>
#include <concepts>

namespace sbx::utility {

template<typename Tag, std::unsigned_integral Type = std::uint32_t, Type InvalidValue = std::numeric_limits<Type>::max()>
class handle final {

public:

  using tag_type = Tag;
  using value_type = Type;

  inline static constexpr value_type invalid_value = InvalidValue;

  handle() noexcept
  : _value{invalid_value} { }

  handle(value_type value) noexcept
  : _value{value} { }

  ~handle() = default;

  operator value_type() const noexcept {
    return _value;
  }

  auto is_valid() const noexcept -> bool {
    return _value != invalid_value;
  }

private:

  value_type _value;

}; // struct handle

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_HANDLE_HPP_
