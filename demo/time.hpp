#ifndef DEMO_TIME_HPP_
#define DEMO_TIME_HPP_

#include <chrono>
#include <concepts>

#include <types/primitives.hpp>

namespace demo {

class time {

  using duration_type = std::chrono::duration<sbx::float32>;

public:

  template<typename Rep, typename Period>
  constexpr time(const std::chrono::duration<Rep, Period>& duration)
  : _value{std::chrono::duration_cast<duration_type>(duration)} { }

  time(const time&) = default;

  ~time() = default;

  time& operator=(const time&) = default;

  constexpr operator sbx::float32() const {
    return _value.count();
  }

private:

  duration_type _value{};

}; // class time

} // namespace demo

#endif // DEMO_TIME_HPP_
