#ifndef LIBSBX_ECS_DETAIL_STORAGE_ITERATOR_HPP_
#define LIBSBX_ECS_DETAIL_STORAGE_ITERATOR_HPP_

#include <iterator>
#include <type_traits>
#include <memory>

#include <libsbx/utility/fast_mod.hpp>

namespace sbx::ecs::detail {

template<typename Container, std::size_t Page>
class storage_iterator final {

  friend storage_iterator<const Container, Page>;

  using container_type = std::remove_const_t<Container>;
  using allocator_traits = std::allocator_traits<typename container_type::allocator_type>;

  using iterator_traits = std::iterator_traits<std::conditional_t<
    std::is_const_v<Container>,
    typename allocator_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::const_pointer,
    typename allocator_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::pointer>
  >;

public: 

  using value_type = typename iterator_traits::value_type;
  using pointer = typename iterator_traits::pointer;
  using reference = typename iterator_traits::reference;
  using difference_type = typename iterator_traits::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  constexpr storage_iterator() noexcept = default;

  constexpr storage_iterator(Container* container, const difference_type offset) noexcept
  : _container{container},
    _offset{offset} { }

  template<bool IsConst = std::is_const_v<Container>>
  requires (!IsConst)
  constexpr storage_iterator(const storage_iterator<std::remove_const_t<Container>, Page>& other) noexcept
  : storage_iterator{other._payload, other._offset} { }

  constexpr auto operator++() noexcept -> storage_iterator& {
    --_offset;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> storage_iterator {
    const auto original = *this;
    ++(*this);
    return original;
  }

  constexpr auto operator--() noexcept -> storage_iterator& {
    ++_offset;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> storage_iterator {
    const auto original = *this;
    --(*this); 
    return original;
  }

  constexpr auto operator+=(const difference_type value) noexcept -> storage_iterator& {
    _offset -= value;
    return *this;
  }

  constexpr auto operator+(const difference_type value) const noexcept -> storage_iterator {
    auto copy = *this;
    return (copy += value);
  }

  constexpr auto operator-=(const difference_type value) noexcept -> storage_iterator& {
    return (*this += -value);
  }

  constexpr auto operator-(const difference_type value) const noexcept -> storage_iterator {
    return (*this + -value);
  }

  [[nodiscard]] constexpr reference operator[](const difference_type value) const noexcept {
    const auto position = static_cast<typename Container::size_type>(index() - value);
    return (*_container)[position / Page][utility::fast_mod(static_cast<std::size_t>(position), Page)];
  }

  [[nodiscard]] constexpr pointer operator->() const noexcept {
    return std::addressof(operator[](0));
  }

  [[nodiscard]] constexpr reference operator*() const noexcept {
    return operator[](0);
  }

  [[nodiscard]] constexpr difference_type index() const noexcept {
    return _offset - 1;
  }

private:

  Container* _container;
  difference_type _offset;

}; // class storage_iterator

} // namespace sbx::ecs::detail

#endif // LIBSBX_ECS_DETAIL_STORAGE_ITERATOR_HPP_
