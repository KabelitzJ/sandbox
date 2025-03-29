#ifndef LIBSBX_VIEW_HPP_
#define LIBSBX_VIEW_HPP_

#include <algorithm>
#include <tuple>
#include <ranges>
#include <vector>
#include <array>

#include <libsbx/utility/type_list.hpp>

#include <libsbx/memory/concepts.hpp>
#include <libsbx/memory/iterable_adaptor.hpp>

#include <libsbx/ecs/type_list.hpp>

#include <libsbx/ecs/detail/view_iterator.hpp>

namespace sbx::ecs {

namespace detail {

template<typename Type, bool IsChecked, std::size_t Get>
requires (std::is_same_v<std::remove_const_t<std::remove_reference_t<Type>>, Type>)
class basic_common_view {
  
  template<typename Return, typename View, typename Other, std::size_t... GetLhs, std::size_t... GetRhs>
  friend auto view_pack(const View&, const Other&, std::index_sequence<GetLhs...>, std::index_sequence<GetRhs...>) -> Return;

public:

  using common_type = Type;
  using entity_type = typename Type::entity_type;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = detail::view_iterator<common_type, IsChecked, Get>;

  auto refresh() noexcept -> void {
    auto position = static_cast<size_type>(_index != Get) * Get;

    for (; position < Get && _pools[position] != nullptr; ++position) { }

    if (position == Get) {
      _unchecked_refresh();
    }
  }

  [[nodiscard]] auto handle() const noexcept -> const common_type* {
    return (_index != Get) ? _pools[_index] : nullptr;
  }

  [[nodiscard]] auto size_hint() const noexcept -> size_type {
    return (_index != Get) ? _offset() : size_type{};
  }

  [[nodiscard]] auto begin() const noexcept -> iterator {
    return (_index != Get) ? iterator{_pools[_index]->end() - static_cast<difference_type>(_offset()), _pools, _index} : iterator{};
  }

  [[nodiscard]] auto end() const noexcept -> iterator {
    return (_index != Get) ? iterator{_pools[_index]->end(), _pools, _index} : iterator{};
  }

  [[nodiscard]] auto front() const noexcept -> entity_type {
    const auto it = begin();
    return it != end() ? *it : null_entity;
  }

  [[nodiscard]] auto back() const noexcept -> entity_type {
    if(_index != Get) {
      auto it = _pools[_index]->rbegin();
      const auto last = it + static_cast<difference_type>(_offset());

      for (const auto idx = static_cast<difference_type>(_index); it != last && !(detail::all_of(_pools.begin(), _pools.begin() + idx, *it) && detail::all_of(_pools.begin() + idx + 1, _pools.end(), *it)); ++it) { }

      return it == last ? null_entity : *it;
    }

    return null_entity;
  }

  [[nodiscard]] auto find(const entity_type entity) const noexcept -> iterator {
    return contains(entity) ? iterator{_pools[_index]->find(entity), _pools, _index} : end();
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return (_index != Get); // && detail::is_fully_initialized(filter.begin(), filter.end(), placeholder);
  }

  [[nodiscard]] auto contains(const entity_type entity) const noexcept -> bool {
    return (_index != Get) && detail::all_of(_pools.begin(), _pools.end(), entity) && _pools[_index]->_index(entity) < _offset();
  }

protected:

  basic_common_view() noexcept
  : _pools{},
    _index{Get} { }

  basic_common_view(std::array<const Type *, Get> value) noexcept
  : _pools{value},
    _index{Get} {
    _unchecked_refresh();
  }

  [[nodiscard]] auto pool_at(const size_type position) const noexcept -> const Type* {
    return _pools[position];
  }

  auto set_pool_at(const size_type position, const common_type* element) noexcept -> void {
    utility::assert_that(element != nullptr, "Unexpected element");
    _pools[position] = element;
    refresh();
  }

  auto use(const size_type position) noexcept -> void {
    _index = (_index != Get) ? position : Get;
  }

private:

  [[nodiscard]] auto _offset() const noexcept {
    utility::assert_that(_index != Get, "Invalid view");
    return (_pools[_index]->policy() == deletion_policy::swap_only) ? _pools[_index]->free_list() : _pools[_index]->size();
  }

  auto _unchecked_refresh() noexcept -> void {
    _index = 0u;

    if constexpr (Get > 1u) {
      for (auto position  = 1u; position < Get; ++position) {
        if (_pools[position]->size() < _pools[_index]->size()) {
          _index = position;
        }
      }
    }
  }

  std::array<const common_type*, Get> _pools;
  size_type _index;

}; // class basic_common_view

} // namespace detail

template<typename... Type>
struct get_t final : type_list<Type...> {
  explicit constexpr get_t() = default;
}; // struct get_t

template<typename... Type>
inline constexpr get_t<Type...> get{};

template<typename... Get>
requires (sizeof...(Get) != 0u)
class basic_view : public detail::basic_common_view<std::common_type_t<typename Get::base_type...>, detail::tombstone_check_v<Get...>, sizeof...(Get)> {

  using base_type = detail::basic_common_view<std::common_type_t<typename Get::base_type...>, detail::tombstone_check_v<Get...>, sizeof...(Get)>;

  template<std::size_t Index>
  using element_at = type_list_element_t<Index, type_list<Get...>>;

  template<typename Type>
  inline static constexpr auto index_of = type_list_index_v<std::remove_const_t<Type>, type_list<typename Get::element_type...>>;

public:

  using common_type = typename base_type::common_type;
  using entity_type = typename base_type::entity_type;
  using size_type = typename base_type::size_type;
  using difference_type = std::ptrdiff_t;
  using iterator = typename base_type::iterator;
  using iterable = memory::iterable_adaptor<detail::extended_view_iterator<iterator, Get...>>;

  basic_view() noexcept
  : base_type{} { }


  basic_view(Get&...value) noexcept
  : base_type{{&value...}} { }

  basic_view(std::tuple<Get&...> value) noexcept
  : basic_view{std::make_from_tuple<basic_view>(value)} { }

  template<typename Type>
  [[nodiscard]] auto* storage() const noexcept {
    return storage<index_of<Type>>();
  }

  template<std::size_t Index>
  [[nodiscard]] auto* storage() const noexcept {
    return static_cast<element_at<Index>*>(const_cast<constness_as_t<common_type, element_at<Index>>*>(base_type::pool_at(Index)));
  }

  template<typename Type>
  auto set_storage(Type& element) noexcept -> void {
    storage<index_of<typename Type::element_type>>(element);
  }

  template<std::size_t Index, typename Type>
  requires (std::is_convertible_v<Type&, element_at<Index>&>)
  void set_storage(Type& element) noexcept {
    base_type::set_pool_at(Index, &element);
  }

  template<typename Type, typename... Other>
  [[nodiscard]] auto get(const entity_type entity) const -> decltype(auto) {
    return get<index_of<Type>, index_of<Other>...>(entity);
  }

  template<std::size_t... Index>
  [[nodiscard]] auto get(const entity_type entity) const -> decltype(auto) {
    if constexpr(sizeof...(Index) == 0) {
      return _get(entity, std::index_sequence_for<Get...>{});
    } else if constexpr(sizeof...(Index) == 1) {
      return (storage<Index>()->get(entity), ...);
    } else {
      return std::tuple_cat(storage<Index>()->get_as_tuple(entity)...);
    }
  }
  
  [[nodiscard]] auto each() const noexcept -> iterable {
    return {base_type::begin(), base_type::end()};
  }

private:

  template<std::size_t... Index>
  [[nodiscard]] auto _get(const entity_type entity, std::index_sequence<Index...>) const noexcept {
    return std::tuple_cat(storage<Index>()->get_as_tuple(entity)...);
  }

}; // class basic_view

template<typename... Type>
basic_view(Type&...storage) -> basic_view<get_t<Type...>>;

template<typename... Get>
basic_view(std::tuple<Get&...>) -> basic_view<get_t<Get...>>;

} // namespace sbx::ecs

#endif // LIBSBX_VIEW_HPP_
