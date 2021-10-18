#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <memory>
#include <utility>
#include <vector>
#include <type_traits>
#include <cassert>
#include <iterator>

#include <types/primitives.hpp>

#include <util/memory.hpp>

#include "entity.hpp"

namespace sbx {
  
template<typename Container>
struct sparse_set_iterator final {
  using value_type = typename Container::value_type;
  using pointer = typename Container::const_pointer;
  using reference = typename Container::const_reference;
  using difference_type = typename Container::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  sparse_set_iterator() noexcept = default;

  sparse_set_iterator(const Container* container, const difference_type index) noexcept
  : _container{container},
    _index{index} {}

  sparse_set_iterator& operator++() noexcept {
    return --_index, *this;
  }

  sparse_set_iterator operator++(const difference_type) noexcept {
    const auto original = *this;
    return ++(*this), original;
  }

  sparse_set_iterator& operator--() noexcept {
    return ++_index, *this;
  }

  sparse_set_iterator operator--(const difference_type) noexcept {
    const auto original = *this;
    return --(*this), original;
  }

  sparse_set_iterator& operator+=(const difference_type value) noexcept {
    _index -= value;
    return *this;
  }

  sparse_set_iterator operator+(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy += value);
  }

  sparse_set_iterator& operator-=(const difference_type value) noexcept {
    _index += value;
    return *this;
  }

  sparse_set_iterator operator-(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy -= value);
  }

  difference_type operator-(const sparse_set_iterator& other) const noexcept {
    return other._index - _index;
  }

  [[nodiscard]] reference operator[](const difference_type value) const {
    return _container[value];
  }

  [[nodiscard]] bool operator==(const sparse_set_iterator& other) const noexcept {
    return other._index == _index;
  }

  [[nodiscard]] bool operator!=(const sparse_set_iterator& other) const noexcept {
    return !(*this == other);
  }

  [[nodiscard]] bool operator<(const sparse_set_iterator& other) const noexcept {
    return _index > other._index;
  }

  [[nodiscard]] bool operator>(const sparse_set_iterator& other) const noexcept {
    return _index < other._index;
  }

  [[nodiscard]] bool operator<=(const sparse_set_iterator& other) const noexcept {
    return !(*this > other);
  }

  [[nodiscard]] bool operator>=(const sparse_set_iterator& other) const noexcept {
    return !(*this < other);
  }

  [[nodiscard]] pointer operator->() const {
    const auto position = _index - 1;
    return _container->data() + position;
  }

  [[nodiscard]] reference operator*() const {
    return *operator->();
  }

private:
  const Container* _container;
  difference_type _index;
};


template<typename Entity, typename = std::allocator<Entity>>
class basic_sparse_set;


template<typename Entity, typename Allocator>
class basic_sparse_set {

  static constexpr auto sparse_page_v = 4096;

  using allocator_traits = std::allocator_traits<Allocator>;
  using alloc = typename allocator_traits::template rebind_alloc<Entity>;
  using alloc_traits = typename std::allocator_traits<alloc>;

  using entity_traits = ::entity_traits<Entity>;

  using sparse_container_type = std::vector<typename alloc_traits::pointer, typename alloc_traits::template rebind_alloc<typename alloc_traits::pointer>>;
  using packed_container_type = std::vector<Entity, alloc>;

public:

  using allocator_type = Allocator;
  using entity_type = Entity;
  using version_type = typename entity_traits::version_type;
  using size_type = typename packed_container_type::size_type;
  using pointer = typename packed_container_type::const_pointer;
  using iterator = sparse_set_iterator<packed_container_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;

protected:

  virtual void _swap_and_pop(const entity_type entity) {
    auto& reference = _sparse_reference(entity);
    const auto position = static_cast<size_type>(entity_traits::to_entity(reference));
    assert(_packed[position] == entity);

    _packed[position] = _packed.back();
    auto& element = _sparse_reference(_packed.back());
    element = entity_traits::combine(entity_traits::to_integral(reference), entity_traits::to_integral(element));
    
    reference = null_entity;
    
    assert((_packed.back() = tombstone_entity, true));
    _packed.pop_back();
  }

private:

  [[nodiscard]] auto* _sparse_pointer(const entity_type entity) const {
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    const auto page = position / sparse_page_v;
    return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod<sparse_page_v>(position)) : nullptr;
  }

  [[nodiscard]] auto& _sparse_reference(const entity_type entity) const {
    assert(_sparse_pointer(entity));
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    return _sparse[position / sparse_page_v][fast_mod<sparse_page_v>(position)];
  }

  void _release_sparse_pages() {
    auto page_allocator = alloc{_packed.get_allocator()};

    for(auto&& page: _sparse) {
      if(page != nullptr) {
        std::destroy(page, page + sparse_page_v);
        alloc_traits::deallocate(page_allocator, page, sparse_page_v);
        page = nullptr;
      }
    }
  }

  sparse_container_type _sparse;
  packed_container_type _packed;
  entity_type _free_list;

};
  
} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
