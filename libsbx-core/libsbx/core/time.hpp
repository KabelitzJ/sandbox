/* 
 * Copyright (c) 2022 Jonas Kabelitz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT/>.
 */

/**
 * @file libsbx/core/time.hpp
 */

#ifndef LIBSBX_CORE_TIME_HPP_
#define LIBSBX_CORE_TIME_HPP_

/**
 * @ingroup libsbx-core
 */

#include <chrono>
#include <concepts>
#include <iostream>

namespace sbx::core {

class time {

  friend constexpr bool operator==(const time& lhs, const time& rhs) noexcept;
  friend constexpr std::partial_ordering operator<=>(const time& lhs, const time& rhs) noexcept;
  friend std::ostream& operator<<(std::ostream& output_stream, const time& time);

  using duration_type = std::chrono::duration<float>;

public:

  constexpr time() = default;

  template<typename Rep, typename Period>
  constexpr time(const std::chrono::duration<Rep, Period>& duration)
  : _value{std::chrono::duration_cast<duration_type>(duration)} { }

  time(const time&) = default;

  ~time() = default;

  time& operator=(const time&) = default;

  constexpr operator float() const {
    return _value.count();
  }

  constexpr time& operator+=(const time& other) noexcept {
    _value += other._value;

    return *this;
  }

  constexpr time& operator-=(const time& other) noexcept {
    _value += other._value;

    return *this;
  }

private:

  duration_type _value{};

}; // class time

[[nodiscard]] constexpr bool operator==(const time& lhs, const time& rhs) noexcept {
  return lhs._value.count() == rhs._value.count();
}

[[nodiscard]] constexpr std::partial_ordering operator<=>(const time& lhs, const time& rhs) noexcept {
  return lhs._value.count() <=> rhs._value.count();
}

[[nodiscard]] constexpr time operator+(time lhs, const time& rhs) noexcept {
  return lhs += rhs;
}

[[nodiscard]] constexpr time operator-(time lhs, const time& rhs) noexcept {
  return lhs -= rhs;
}

[[nodiscard]] std::ostream& operator<<(std::ostream& output_stream, const time& time) {
  return output_stream << time._value.count();
}

} // namespace sbx::core

#endif // LIBSBX_CORE_TIME_HPP_
