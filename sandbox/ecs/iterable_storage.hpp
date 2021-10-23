#ifndef SBX_ECS_ITERABLE_STORAGE_HPP_
#define SBX_ECS_ITERABLE_STORAGE_HPP_

#include <tuple>
#include <type_traits>

#include <types/type_traits.hpp>

#include "component.hpp"

namespace sbx {

template<typename Entity, typename Component>
class iterable_storage final {

  using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;
  using basic_common_type = typename storage_type::base_type;

  template<typename... Iterators>
  struct iterable_storage_iterator final {

    using difference_type = std::ptrdiff_t;
    using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<decltype(std::declval<storage_type&>().get_as_tuple({}))>()));
    using pointer = void;
    using reference = value_type;
    using iterator_category = std::input_iterator_tag;

    template<typename... Discard>
    iterable_storage_iterator(Iterators... from, Discard...) noexcept
    : _iterators{from...} { }

    iterable_storage_iterator& operator++() noexcept {
      return (++std::get<Iterators>(_iterators), ...), *this;
    }

    iterable_storage_iterator operator++(int) noexcept {
      const auto original = *this;
      return ++(*this), original;
    }

    [[nodiscard]] reference operator*() const noexcept {
      return {*std::get<Iterators>(_iterators)...};
    }

    [[nodiscard]] bool operator==(const iterable_storage_iterator& other) const noexcept {
      return std::get<0>(other._iterators) == std::get<0>(_iterators);
    }

    [[nodiscard]] bool operator!=(const iterable_storage_iterator& other) const noexcept {
      return !(*this == other);
    }

  private:

    std::tuple<Iterators...> _iterators{};

  };

public:

  using iterator = std::conditional_t<
    ignore_as_empty_v<std::remove_const_t<Component>>,
    iterable_storage_iterator<typename basic_common_type::iterator>,
    iterable_storage_iterator<typename basic_common_type::iterator, decltype(std::declval<storage_type>().begin())>
  >;
  using reverse_iterator = std::conditional_t<
    ignore_as_empty_v<std::remove_const_t<Component>>,
    iterable_storage_iterator<typename basic_common_type::reverse_iterator>,
    iterable_storage_iterator<typename basic_common_type::reverse_iterator, decltype(std::declval<storage_type>().rbegin())>
  >;

  iterable_storage(storage_type* pool)
  : _pool{pool} {}

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

  const storage_type* _pool{};

};

} // namespace sbx

#endif // SBX_ECS_ITERABLE_STORAGE_HPP_
