#ifndef LIBSBX_ANIMATIONS_SPLINE_HPP_
#define LIBSBX_ANIMATIONS_SPLINE_HPP_

#include <vector>
#include <algorithm>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/quaternion.hpp>

namespace sbx::animations {

template<typename Type>
class spline {

public:

  auto add(const std::float_t timestamp, const Type& value) -> void {
    _timestamps.push_back(timestamp);
    _values.push_back(value);
  }

  auto sample(const std::float_t time) const -> Type {
    while (_current_index + 1u < _timestamps.size() && time > _timestamps[_current_index + 1]) {
      ++_current_index;
    }

    if (_current_index + 1u >= _timestamps.size()) {
      _current_index = 0u;
    }

    const auto t = (time - _timestamps[_current_index]) / (_timestamps[_current_index + 1u] - _timestamps[_current_index]);

    if constexpr (std::is_same_v<Type, math::quaternion>) {
      return math::quaternion::slerp(_values[_current_index], _values[_current_index + 1], t);
    } else {
      return math::vector3::lerp(_values[_current_index], _values[_current_index + 1], t);
    }
  }

private:

  std::vector<std::float_t> _timestamps;
  std::vector<Type> _values;
  mutable std::uint32_t _current_index;

}; // class spline

} // namespace sbx::animations

#endif // LIBSBX_ANIMATIONS_SPLINE_HPP_