#ifndef LIBSBX_VIEW_HPP_
#define LIBSBX_VIEW_HPP_

#include <algorithm>
#include <tuple>
#include <ranges>
#include <vector>
#include <array>

#include <libsbx/utility/type_list.hpp>

#include <libsbx/memory/concepts.hpp>

#include <libsbx/ecs/iterable_adaptor.hpp>

namespace sbx::ecs {

namespace detail {

template<typename Container, std::size_t Size>
class view_iterator {

  using iterator_type = typename Container::const_iterator;

public:

  using value_type = typename iterator_type::value_type;
  using pointer = typename iterator_type::pointer;
  using reference = typename iterator_type::reference;
  using difference_type = typename iterator_type::difference_type;
  using iterator_category = std::forward_iterator_tag;

  constexpr view_iterator() noexcept
  : _current{},
    _end{},
    _containers{} { }

  view_iterator(iterator_type current, iterator_type end, std::array<const Container*, Size> containers) noexcept
  : _current{current},
    _end{end},
    _containers{containers} {
    while(_current != _end && !_is_valid()) {
      ++_current;
    }
  }

  auto operator++() noexcept -> view_iterator& {
    while(++_current != _end && !_is_valid()) {}
    return *this;
  }

  auto operator++(int) noexcept -> view_iterator {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  auto operator->() const noexcept -> pointer {
    return &*_current;
  }

  auto operator*() const noexcept -> reference {
    return *(operator->());
  }

  template<typename LhsContainer, std::size_t LhsSize, typename RhsContainer, std::size_t RhsSize>
  friend auto operator==(const view_iterator<LhsContainer, LhsSize>& lhs, const view_iterator<RhsContainer, RhsSize>& rhs) noexcept -> bool {
    return lhs._current == rhs._current;
  } 

private:

  auto _is_valid() const noexcept -> bool {
    return (Size != 0u) && std::apply([entity = *_current](const auto*... container){ return (container->contains(entity) && ...); }, _containers);
  }

  iterator_type _current;
  iterator_type _end;
  std::array<const Container*, Size> _containers;

}; // class view_iterator

template<typename Type>
struct input_iterator_pointer final {

  using value_type = Type;
  using pointer = value_type*;
  using reference = value_type&;

  constexpr input_iterator_pointer(value_type&& value) noexcept(std::is_nothrow_move_constructible_v<value_type>)
  : _value{std::move(value)} {}


  [[nodiscard]] constexpr pointer operator->() noexcept {
    return std::addressof(_value);
  }

  [[nodiscard]] constexpr reference operator*() noexcept {
    return _value;
  }

private:

  value_type _value;

}; // struct input_iterator_pointer

template<typename Iterator, typename... Types>
class extended_view_iterator final {

public:

  using iterator_type = Iterator;
  using difference_type = std::ptrdiff_t;
  using value_type = decltype(std::tuple_cat(std::make_tuple(*std::declval<Iterator>()), std::declval<Types>().as_tuple({})...));
  using pointer = input_iterator_pointer<value_type>;
  using reference = value_type;
  using iterator_category = std::input_iterator_tag;

  constexpr extended_view_iterator()
  : _iterator{},
    _containers{} {}

  extended_view_iterator(iterator_type iterator, std::tuple<Types*...> containers)
  : _iterator{iterator},
    _containers{containers} {}

  extended_view_iterator &operator++() noexcept {
    ++_iterator;
    return *this;
  }

  extended_view_iterator operator++(int) noexcept {
    auto copy = *this;
    ++(*this); 
    return copy;
  }

  [[nodiscard]] reference operator*() const noexcept {
    return std::apply([current = *_iterator](auto*... container) { return std::tuple_cat(std::make_tuple(current), container->as_tuple(current)...); }, _containers);
  }

  [[nodiscard]] pointer operator->() const noexcept {
    return operator*();
  }

  [[nodiscard]] constexpr iterator_type base() const noexcept {
    return _iterator;
  }

  template<typename... Lhs, typename... Rhs>
  friend bool constexpr operator==(const extended_view_iterator<Lhs...>& lhs, const extended_view_iterator<Rhs...>& rhs) noexcept {
    return lhs._iterator == rhs._iterator;
  }

private:

  iterator_type _iterator;
  std::tuple<Types*...> _containers;

}; // class extended_view_iterator

} // namespace detail

template<typename... Containers>
class basic_view {

  using underlying_type = std::common_type_t<typename Containers::key_type...>;
  using basic_common_type = std::common_type_t<typename Containers::base_type...>;

  using container_storage_type = std::tuple<Containers*...>;

  template<typename Type>
  inline static constexpr auto index_of = utility::type_list_index_v<std::remove_const_t<Type>, utility::type_list<typename Containers::value_type...>>; 

  template<typename Entity, memory::allocator_for<Entity> Allocator>
  friend class basic_registry;

public:

  using entity_type = underlying_type;
  using size_type = std::size_t;
  using base_type = basic_common_type;
  using iterator = detail::view_iterator<base_type, sizeof...(Containers) - 1u>;
  using iterable = iterable_adaptor<detail::extended_view_iterator<iterator, Containers...>>;

  ~basic_view() = default;
  
  auto begin() const noexcept -> iterator {
    return iterator{handle().begin(), handle().end(), _check()};
  }

  auto end() const noexcept -> iterator {
    return iterator{handle().end(), handle().end(), _check()};
  }

  auto handle() const noexcept -> const base_type& {
    return *_view;
  }

  template<typename Type>
  auto storage() const noexcept -> decltype(auto) {
    return storage<index_of<Type>>();
  }

  template<std::size_t Index>
  requires (Index < std::tuple_size_v<container_storage_type>)
  auto storage() const noexcept -> decltype(auto) {
    return *std::get<Index>(_containers);
  }

  template<typename... Types>
  auto get(const entity_type entity) const -> decltype(auto) {
    if constexpr(sizeof...(Types) == 0) {
      return std::apply([entity](auto*... container) { return std::tuple_cat(container->as_tuple(entity)...); }, _containers);
    } else if constexpr(sizeof...(Types) == 1) {
      return (storage<index_of<Types>>().get(entity), ...);
    } else {
      return std::tuple_cat(storage<index_of<Types>>().as_tuple(entity)...);
    }
  }

  auto each() const noexcept -> iterable {
    return iterable{detail::extended_view_iterator{begin(), _containers}, detail::extended_view_iterator{end(), _containers}};
  }

private:

  basic_view() noexcept = default;

  basic_view(Containers&... containers) noexcept
  : _containers{&containers...},
    _view{std::get<0>(_containers)} {
    ((_view = containers.size() < _view->size() ? &containers : _view), ...);
  }

  auto _check() const noexcept -> std::array<const base_type*, sizeof...(Containers) - 1u> {
    auto result = std::array<const base_type*, sizeof...(Containers) - 1u>{};
    std::apply([&result, position = 0u, view = _view](const auto*... container) mutable { 
      ((container == view ? void() : void(result[position++] = container)), ...); 
    }, _containers);
    return result;
  }

  container_storage_type _containers;
  const base_type* _view;

}; // class basic_view

template<typename Container>
class basic_view<Container> {

  template<typename Entity, memory::allocator_for<Entity> Allocator>
  friend class basic_registry;

public:

  using entity_type = typename Container::key_type;
  using size_type = std::size_t;
  using base_type = typename Container::base_type;
  using iterator = typename base_type::const_iterator;

  basic_view() noexcept
  : _container{},
    _view{} { }

  basic_view(Container& container) noexcept
  : _container{&container},
    _view{&container} { }

  auto begin() const noexcept -> iterator {
    return handle().begin();
  }

  auto end() const noexcept -> iterator {
    return handle().end();
  }

  auto handle() const noexcept -> const base_type& {
    return *_view;
  }

  template<typename Type = typename Container::value_type>
  auto storage() const noexcept -> decltype(auto) {
    return storage<0>();
  }

  template<std::size_t Index>
  auto storage() const noexcept -> decltype(auto) {
    return *std::get<Index>(_container);
  }

  template<typename... Types>
  auto get(const entity_type entity) const -> decltype(auto) {
    return storage().get(entity);
  }

private:

  std::tuple<Container*> _container;
  const base_type* _view;

}; // class basic_view

} // namespace sbx::ecs

#endif // LIBSBX_VIEW_HPP_
