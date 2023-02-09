#ifndef DEMO_QUANTITY_HPP_
#define DEMO_QUANTITY_HPP_

#include <ratio>
#include <cmath>
#include <typeindex>
#include <iostream>

namespace demo {

template<typename Type>
concept representation = (std::is_integral_v<Type> && !std::is_same_v<Type, bool> ) || std::is_floating_point_v<Type>;

template<typename Type>
concept ratio = requires {
  { Type::num } -> std::convertible_to<std::intmax_t>;
  { Type::den } -> std::convertible_to<std::intmax_t>;
};

template<representation Representation, ratio Ratio, ratio OtherRatio>
struct ratio_conversion {
  static constexpr auto value = 
    static_cast<Representation>(OtherRatio::num) / static_cast<Representation>(OtherRatio::den) *
    static_cast<Representation>(Ratio::den) / static_cast<Representation>(Ratio::num);
}; // struct ratio_conversion

template<representation Representation, ratio R1, ratio R2>
inline static constexpr auto ratio_conversion_v = ratio_conversion<Representation, R1, R2>::value;

template<typename Dimension, representation Representation, ratio Ratio = std::ratio<1, 1>>
class quantity {

public:

  using dimension_type = Dimension;
  using value_type = Representation;
  using ratio_type = Ratio;

  quantity() = default;

  explicit quantity(value_type value)
  : _value{value} { }

  template<representation OtherRepresentation, ratio OtherRatio = ratio_type>
  quantity(const quantity<dimension_type, OtherRepresentation, OtherRatio>& other)
  : _value{static_cast<value_type>(other.value()) * ratio_conversion_v<value_type, Ratio, OtherRatio>} { }

  quantity(const quantity& other) = default;

  quantity(quantity&& other) = default;

  ~quantity() = default;

  auto operator=(const quantity& other) -> quantity& = default;

  auto operator=(quantity&& other) -> quantity& = default;

  auto value() const -> value_type {
    return _value;
  }

private:

  value_type _value{};

}; // class quantity

template<typename TargetQuantity, representation FromRepresentation, ratio FromRatio>
auto quantity_cast(const quantity<typename TargetQuantity::dimension_type, FromRepresentation, FromRatio>& from) -> TargetQuantity {
  using value_type = typename TargetQuantity::value_type;
  using ratio_type = typename TargetQuantity::ratio_type;

  return TargetQuantity{static_cast<value_type>(from.value()) * ratio_conversion_v<value_type, ratio_type, FromRatio>};
}

} // namespace demo

#endif // DEMO_QUANTITY_HPP_
