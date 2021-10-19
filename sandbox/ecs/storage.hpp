#ifndef SBX_ECS_STORAGE_HPP_
#define SBX_ECS_STORAGE_HPP_

#include <iterator>
#include <type_traits>

#include <util/memory.hpp>

#include "component.hpp"
#include "sparse_set.hpp"

namespace sbx {

template<typename Container>
class storage_iterator final {

  static constexpr auto packed_page_v = 1024;

  using container_type = std::remove_const_t<Container>;
  using allocator_traits = std::allocator_traits<typename container_type::allocator_type>;

  using iterator_traits = std::iterator_traits<std::conditional_t<
    std::is_const_v<Container>,
    typename allocator_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::const_pointer,
    typename allocator_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::pointer>
  >;

public:
  using difference_type = typename iterator_traits::difference_type;
  using value_type = typename iterator_traits::value_type;
  using pointer = typename iterator_traits::pointer;
  using reference = typename iterator_traits::reference;
  using iterator_category = std::random_access_iterator_tag;

  storage_iterator() noexcept = default;

  storage_iterator(Container& packed, difference_type index) noexcept
  : _packed{packed},
    _index{index} {}

  storage_iterator& operator++() noexcept {
    return --_index, *this;
  }

  storage_iterator operator++(int) noexcept {
    const auto original = *this;
    return ++(*this), original;
  }

  storage_iterator& operator--() noexcept {
      return ++_index, *this;
  }

  storage_iterator operator--(int) noexcept {
    const auto original = *this;
    return --(*this), original;
  }

  storage_iterator& operator+=(const difference_type value) noexcept {
    _index -= value;
    return *this;
  }

  storage_iterator operator+(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy += value);
  }

  storage_iterator& operator-=(const difference_type value) noexcept {
    _index += value;
    return *this;
  }

  storage_iterator operator-(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy -= value);
  }

  difference_type operator-(const storage_iterator& other) const noexcept {
    return other._index - _index;
  }

  [[nodiscard]] reference operator[](const difference_type value) const noexcept {
      return *operator+(value);
  }

  [[nodiscard]] bool operator==(const storage_iterator& other) const noexcept {
      return other._index == _index;
  }

  [[nodiscard]] bool operator!=(const storage_iterator& other) const noexcept {
      return !(*this == other);
  }

  [[nodiscard]] bool operator<(const storage_iterator& other) const noexcept {
      return _index > other._index;
  }

  [[nodiscard]] bool operator>(const storage_iterator& other) const noexcept {
    return _index < other._index;
  }

  [[nodiscard]] bool operator<=(const storage_iterator& other) const noexcept {
    return !(*this > other);
  }

  [[nodiscard]] bool operator>=(const storage_iterator& other) const noexcept {
    return !(*this < other);
  }

  [[nodiscard]] pointer operator->() const noexcept {
    const auto position = _index - 1;
    return (*_packed)[position / packed_page_v] + fast_mod<packed_page_v>(position);
  }

  [[nodiscard]] reference operator*() const noexcept {
    return *operator->();
  }

private:
  Container& _packed{};
  difference_type _index{};
}; // class storage_iterator


template<typename, typename Type, typename = std::allocator<Type>, typename = void>
class basic_storage;


template<typename Entity, typename Type, typename Allocator, typename>
class basic_storage {

  static constexpr auto packed_page_v = 1024;

  using allocator_traits = std::allocator_traits<Allocator>;
  using alloc = typename allocator_traits::template rebind_alloc<Type>;
  using alloc_traits = typename std::allocator_traits<alloc>;

  using entity_traits = sbx::entity_traits<Entity>;
  using comp_traits = component_traits<Type>;
  using underlying_type = basic_sparse_set<Entity, typename allocator_traits::template rebind_alloc<Entity>>;
  using container_type = std::vector<typename alloc_traits::pointer, typename alloc_traits::template rebind_alloc<typename alloc_traits::pointer>>;

public:
  using base_type = underlying_type;
  using allocator_type = Allocator;
  using value_type = Type;
  using entity_type = Entity;
  using size_type = std::size_t;
  using pointer = typename container_type::pointer;
  using const_pointer = typename alloc_traits::template rebind_traits<typename alloc_traits::const_pointer>::const_pointer;
  using iterator = storage_iterator<container_type>;
  using const_iterator = storage_iterator<const container_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;


protected:

private:

  [[nodiscard]] auto& _element_at(const size_type position) const {
    return _packed.first[position / packed_page_v][fast_mod<packed_page_v>(position)];
  }

  auto _assure_atleast(const size_type position) {
    auto& container = _packed.first;
    auto& allocator = _packed.second;
    const auto index = position / packed_page_v;

    if (index >= container.size()) {
      auto current = container.size();
      container.resize(index + 1u, nullptr);

      for (const auto last = container.size(); current < last; ++current) {
        container[current] = alloc_traits::allocate(allocator, packed_page_v);
      }
      
      return container[index] + fast_mod<packed_page_v>(position);
    }
  }

  std::pair<container_type, alloc> _packed{};

}; // class basic_storage

template<typename Entity, typename Type, typename Allocator>
class basic_storage<Entity, Type, Allocator, std::enable_if_t<ignore_as_empty_v<Type>>> 
: public basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>> {

public:

private:

}; // class basic_storage

template<typename Entity, typename Type, typename = void>
struct storage_traits {
  using storage_type = basic_storage<Entity, Type>;
};

} // namespace sbx

#endif // SBX_ECS_STORAGE_HPP_
