#ifndef LIBSBX_ITERABLE_ADAPTOR_HPP_
#define LIBSBX_ITERABLE_ADAPTOR_HPP_

#include <concepts>
#include <iterator>

namespace sbx::ecs {

template<std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel = Iterator>
class iterable_adaptor final {

public:

  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using iterator = Iterator;
  using sentinel = Sentinel;

  constexpr iterable_adaptor() noexcept(std::is_nothrow_default_constructible_v<iterator> && std::is_nothrow_default_constructible_v<sentinel>)
  : _begin{},
    _end{} {}

  constexpr iterable_adaptor(iterator begin, sentinel end) noexcept(std::is_nothrow_move_constructible_v<iterator> && std::is_nothrow_move_constructible_v<sentinel>)
  : _begin{std::move(begin)},
    _end{std::move(end)} {}

  constexpr auto begin() const noexcept -> iterator {
    return _begin;
  }

  constexpr auto end() const noexcept -> sentinel {
    return _end;
  }

private:

  iterator _begin;
  sentinel _end;

}; // class iterable_adaptor

} // namespace sbx::ecs

#endif // LIBSBX_ITERABLE_ADAPTOR_HPP_
