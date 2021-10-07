#ifndef SBX_ECS_ITERABLE_STORAGE_HPP_
#define SBX_ECS_ITERABLE_STORAGE_HPP_

#include <tuple>
#include <type_traits>

#include <types/type_traits.hpp>

#include "storage.hpp"

namespace sbx {

template<typename Entity, typename Storage, typename... Iterables>
struct iterable_storage_iterator final {
  using difference_type = std::ptrdiff_t;
  using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<decltype(std::declval<Storage&>().get_as_tuple({}))>()));
  using pointer = void;
  using reference = value_type;
  using iterator_category = std::input_iterator_tag;

  template<typename... Discard>
  iterable_storage_iterator(Iterables... from, Discard...) noexcept
  : _iterables{from...} { }

  iterable_storage_iterator& operator++() noexcept {
    return (++std::get<Iterables>(_iterables), ...), *this;
  }

  iterable_storage_iterator operator++(int) noexcept {
    auto original = *this;
    return ++(*this), original;
  }

  [[nodiscard]] reference operator*() const noexcept {
    return {*std::get<Iterables>(_iterables)...};
  }

  [[nodiscard]] bool operator==(const iterable_storage_iterator& other) const noexcept {
    return std::get<0>(other._iterables) == std::get<0>(_iterables);
  }

  [[nodiscard]] bool operator!=(const iterable_storage_iterator& other) const noexcept {
    return !(*this == other);
  }

private:
  std::tuple<Iterables...> _iterables{};

};


template<typename Entity, typename Component>
class iterable_storage final {

  using storage_type = constness_as_t<Component, basic_storage<Entity, std::remove_const_t<Component>>>;
  using basic_common_type = typename storage_type::base_type;

public:
  using iterator = std::conditional_t<
      ignore_as_empty_v<std::remove_const_t<Component>>,
      iterable_storage_iterator<Entity, storage_type, typename basic_common_type::iterator>,
      iterable_storage_iterator<Entity, storage_type, typename basic_common_type::iterator, decltype(std::declval<storage_type>().begin())>>;
  using reverse_iterator = std::conditional_t<
      ignore_as_empty_v<std::remove_const_t<Component>>,
      iterable_storage_iterator<Entity, storage_type, typename basic_common_type::reverse_iterator>,
      iterable_storage_iterator<Entity, storage_type, typename basic_common_type::reverse_iterator, decltype(std::declval<storage_type>().rbegin())>>;

  iterable_storage(storage_type& ref)
  : _pool{&ref} { }

  [[nodiscard]] iterator begin() const noexcept {
    return iterator{_pool->basic_common_type::begin(), _pool->begin()};
  }

  [[nodiscard]] iterator end() const noexcept {
    return iterator{_pool->basic_common_type::end(), _pool->end()};
  }

  [[nodiscard]] reverse_iterator rbegin() const noexcept {
    return reverse_iterator{_pool->basic_common_type::rbegin(), _pool->rbegin()};
  }

  [[nodiscard]] reverse_iterator rend() const noexcept {
    return reverse_iterator{_pool->basic_common_type::rend(), _pool->rend()};
  }

private:
  storage_type* _pool{};

};

} // namespace sbx

#endif // SBX_ECS_ITERABLE_STORAGE_HPP_
