#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <vector>
#include <utility>
#include <iterator>
#include <memory>
#include <cassert>
#include <iterator>
#include <type_traits>

#include <types/primitives.hpp>

#include <util/memory.hpp>

#include "entity.hpp"

namespace sbx {
  
template<typename Traits>
class sparse_set_iterator final {

public:
  using value_type = typename Traits::value_type;
  using pointer = typename Traits::pointer;
  using reference = typename Traits::reference;
  using difference_type = typename Traits::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  sparse_set_iterator() noexcept = default;

  sparse_set_iterator(const pointer* ref, const difference_type idx) noexcept
  : _packed{ref},
    _index{idx} {}

  ~sparse_set_iterator() = default;

  sparse_set_iterator& operator++() noexcept {
    return --_index, *this;
  }

  sparse_set_iterator operator++(int) noexcept {
    sparse_set_iterator original = *this;
    return ++(*this), original;
  }

  sparse_set_iterator& operator--() noexcept {
    return ++_index, *this;
  }

  sparse_set_iterator operator--(int) noexcept {
    sparse_set_iterator original = *this;
    return operator--(), original;
  }

  sparse_set_iterator& operator+=(const difference_type value) noexcept {
    _index -= value;
    return *this;
  }

  sparse_set_iterator operator+(const difference_type value) const noexcept {
    sparse_set_iterator copy = *this;
    return (copy += value);
  }

  sparse_set_iterator& operator-=(const difference_type value) noexcept {
    return (*this += -value);
  }

  sparse_set_iterator operator-(const difference_type value) const noexcept {
    return (*this + -value);
  }

  difference_type operator-(const sparse_set_iterator& other) const noexcept {
    return other._index - _index;
  }

  [[nodiscard]] reference operator[](const difference_type value) const {
    return *operator+(value);
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
    return (*_packed) + position;
  }

  [[nodiscard]] reference operator*() const {
    return *operator->();
  }

private:
  const pointer* _packed;
  difference_type _index;

}; // struct sparse_set_iterator


template<typename Entity, typename Allocator = std::allocator<Entity>>
class basic_sparse_set {

  static constexpr auto growth_factor_v = 1.5f;
  static constexpr auto sparse_page_v = 4096u;

  using allocator_traits = std::allocator_traits<Allocator>;

  using alloc = typename allocator_traits::template rebind_alloc<Entity>;
  using alloc_traits = typename std::allocator_traits<alloc>;
  using alloc_const_pointer = typename alloc_traits::const_pointer;
  using alloc_pointer = typename alloc_traits::pointer;

  using alloc_ptr = typename allocator_traits::template rebind_alloc<alloc_pointer>;
  using alloc_ptr_traits = typename std::allocator_traits<alloc_ptr>;
  using alloc_ptr_pointer = typename alloc_ptr_traits::pointer;

  using entity_traits = sbx::entity_traits<Entity>;

public:

  using entity_type = Entity;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using version_type = typename entity_traits::version_type;
  using pointer = alloc_const_pointer;
  using iterator = sparse_set_iterator<std::iterator_traits<pointer>>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  basic_sparse_set(const allocator_type& allocator = {})
  : _reserved{allocator, size_type{}},
    _sparse{},
    _packed{},
    _bucket{},
    _count{},
    _free_list{tombstone} {}

  basic_sparse_set(const basic_sparse_set&) = delete;

  basic_sparse_set(basic_sparse_set&& other) noexcept
  : _reserved{std::move(other._reserved.first), std::exchange(other._reserved.second, size_type{})},
    _sparse{std::exchange(other._sparse, alloc_ptr_pointer{})},
    _packed{std::exchange(other._packed, alloc_pointer{})},
    _bucket{std::exchange(other._bucket, size_type{})},
    _count{std::exchange(other._count, size_type{})},
    _free_list{std::exchange(other._free_list, tombstone)} {}

  virtual ~basic_sparse_set() {
    _release_memory();
  }

  basic_sparse_set& operator=(const basic_sparse_set&) = delete;

  basic_sparse_set& operator=(basic_sparse_set &&other) noexcept {
    _release_memory();

    assert(alloc_traits::is_always_equal::value || _reserved.first == other._reserved.first);

    _reserved.second = std::exchange(other._reserved.second, size_type{});
    _sparse = std::exchange(other._sparse, alloc_ptr_pointer{});
    _packed = std::exchange(other._packed, alloc_pointer{});
    _bucket = std::exchange(other._bucket, size_type{});
    _count = std::exchange(other._count, size_type{});
    _free_list = std::exchange(other._free_list, tombstone);

    return *this;
  }

  [[nodiscard]] size_type slot() const noexcept {
    return _free_list == null ? _count : static_cast<size_type>(entity_traits::to_entity(_free_list));
  }

  virtual void reserve(const size_type capacity) {
    if(capacity > _reserved.second) {
      _resize_packed(capacity);
    }
  }

  [[nodiscard]] virtual size_type capacity() const noexcept {
    return _reserved.second;
  }

  virtual void shrink_to_fit() {
    if(_count < _reserved.second) {
      _resize_packed(_count);
    }
  }

  [[nodiscard]] size_type extent() const noexcept {
    return _bucket * sparse_page_v;
  }

  [[nodiscard]] size_type size() const noexcept {
    return _count;
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return (_count == size_type{0u});
  }

  [[nodiscard]] pointer data() const noexcept {
    return _packed;
  }

  [[nodiscard]] iterator begin() const noexcept {
    const auto position = static_cast<typename iterator::difference_type>(_count);
    return iterator{std::addressof(_packed), position};
  }

  [[nodiscard]] iterator end() const noexcept {
    return iterator{std::addressof(_packed), {}};
  }

  [[nodiscard]] reverse_iterator rbegin() const noexcept {
    return std::make_reverse_iterator(end());
  }

  [[nodiscard]] reverse_iterator rend() const noexcept {
    return std::make_reverse_iterator(begin());
  }

  [[nodiscard]] iterator find(const entity_type entity) const noexcept {
    return contains(entity) ? --(end() - index(entity)) : end();
  }

  [[nodiscard]] bool contains(const entity_type entity) const noexcept {
    if(auto element = _sparse_pointer(entity); element) {
      constexpr auto capacity = entity_traits::to_entity(null);

      return (((~capacity & entity_traits::to_integral(entity)) ^ entity_traits::to_integral(*element)) < capacity);
    }

    return false;
  }

  [[nodiscard]] version_type current_version(const entity_type entity) const {
    if(auto element = _sparse_pointer(entity); element) {
      return entity_traits::to_version(*element);
    }

    return entity_traits::to_version(tombstone);
  }

  [[nodiscard]] size_type index(const entity_type entity) const noexcept {
    assert(contains(entity));
    return static_cast<size_type>(entity_traits::to_entity(_sparse_reference(entity)));
  }

  [[nodiscard]] entity_type at(const size_type position) const noexcept {
    return position < _count ? _packed[position] : null;
  }

  [[nodiscard]] entity_type operator[](const size_type position) const noexcept {
    assert(position < _count);
    return _packed[position];
  }

  void emplace(const entity_type entity) {
    try_emplace(entity);
    assert(contains(entity));
  }

  template<typename Iterator>
  void insert(Iterator first, Iterator last) {
    for(; first != last && _free_list != null; ++first) {
      emplace(*first);
    }

    reserve(_count + std::distance(first, last));

    for(; first != last; ++first) {
      emplace(*first);
    }
  }

  void erase(const entity_type entity) {
    assert(contains(entity));
    swap_and_pop(entity);
    assert(!contains(entity));
  }

  template<typename Iterator>
  void erase(Iterator first, Iterator last) {
    for (; first != last; ++first) {
      erase(*first);
    }
  }

  bool remove(const entity_type entity) {
    return contains(entity) && (erase(entity), true);
  }

  template<typename Iterator>
  size_type remove(Iterator first, Iterator last) {
    auto found = size_type{};

    for(; first != last; ++first) {
      if (remove(*first)) {
        ++found;
      }
    }

    return found;
  }

  void compact() {
    auto next = _count;

    for(; next && _packed[next - 1u] == tombstone; --next);

    for(auto *itr = &_free_list; *itr != null && next; itr = std::addressof(_packed[entity_traits::to_entity(*itr)])) {
      if(const auto position = entity_traits::to_entity(*itr); position < next) {
        --next;
        move_and_pop(next, position);
        std::swap(_packed[next], _packed[position]);
        const auto entity = static_cast<typename entity_traits::entity_type>(position);
        _sparse_reference(_packed[position]) = entity_traits::combine(entity, entity_traits::to_integral(_packed[position]));
        *itr = entity_traits::combine(static_cast<typename entity_traits::entity_type>(next), entity_traits::reserved);
        for(; next && _packed[next - 1u] == tombstone; --next);
      }
    }

    _free_list = tombstone;
    _count = next;
  }

  void clear() {
    for(auto&& entity: *this) {
      remove(entity);
    }
  }

protected:

  virtual void swap_at(const std::size_t, const std::size_t) {}

  virtual void move_and_pop(const std::size_t, const std::size_t) {}

  virtual void swap_and_pop(const Entity entity) {
    auto& reference = _sparse_reference(entity);
    const auto position = static_cast<size_type>(entity_traits::to_entity(reference));

    assert(_packed[position] == entity);

    auto& last = _packed[--_count];

    _packed[position] = last;
    auto& element = _sparse_reference(last);
    element = entity_traits::combine(entity_traits::to_integral(reference), entity_traits::to_integral(element));
    
    
    reference = null;
    
    assert((last = tombstone, true));
  }

  virtual void try_emplace(const Entity entity) {
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    const auto page = position / sparse_page_v;

    if(!(page < _bucket)) {
      const auto size = size_type{page + 1u};
      auto allocator_ptr = alloc_ptr{_reserved.first};
      const auto memory = alloc_ptr_traits::allocate(allocator_ptr, size);

      std::uninitialized_value_construct(memory + _bucket, memory + size);

      if(_sparse) {
        std::uninitialized_copy(_sparse, _sparse + _bucket, memory);
        std::destroy(_sparse, _sparse + _bucket);
        alloc_ptr_traits::deallocate(allocator_ptr, _sparse, _bucket);
      }

      _sparse = memory;
      _bucket = size;
    }

    if(!_sparse[page]) {
      _sparse[page] = alloc_traits::allocate(_reserved.first, sparse_page_v);
      std::uninitialized_fill(_sparse[page], _sparse[page] + sparse_page_v, null);
    }

    auto& element = _sparse[page][fast_mod<sparse_page_v>(position)];
    assert(entity_traits::to_version(element) == entity_traits::to_version(tombstone));

    if(_free_list == null) {
      if(_count == _reserved.second) {
        const auto size = static_cast<size_type>(static_cast<decltype(growth_factor_v)>(_reserved.second) * growth_factor_v);
        _resize_packed(size + !(size > _reserved.second));
      }

      element = entity_traits::combine(static_cast<typename entity_traits::entity_type>(_count), entity_traits::to_integral(entity));
      _packed[_count++] = entity;
    } else {
      element = entity_traits::combine(entity_traits::to_integral(_free_list), entity_traits::to_integral(entity));
      _free_list = std::exchange(_packed[static_cast<size_type>(entity_traits::to_entity(_free_list))], entity);
    }
  }


private:

  [[nodiscard]] auto _sparse_pointer(const Entity entity) const {
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    const auto page = position / sparse_page_v;

    return (page < _bucket && _sparse[page]) ? (_sparse[page] + fast_mod<sparse_page_v>(position)) : alloc_pointer{};
  }

  [[nodiscard]] auto& _sparse_reference(const Entity entity) const {
    assert(_sparse_pointer(entity));

    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));

    return _sparse[position / sparse_page_v][fast_mod<sparse_page_v>(position)];
  }

  void _resize_packed(const size_type request) {
    assert((request != _reserved.second) && !(request < _count));

    const auto memory = alloc_traits::allocate(_reserved.first, request);

    std::uninitialized_fill(memory + _count, memory + request, tombstone);

    if (_packed) {
      std::uninitialized_copy(_packed, _packed + _count, memory);
      std::destroy(_packed, _packed + _reserved.second);
      alloc_traits::deallocate(_reserved.first, _packed, _reserved.second);
    }

    _packed = memory;
    _reserved.second = request;
  }

  void _release_memory() {
    if(_packed) {
      std::destroy(_packed, _packed + _reserved.second);
      alloc_traits::deallocate(_reserved.first, _packed, _reserved.second);
    }

    if (_sparse) {
      for (auto position = size_type{}; position < _bucket; ++position) {
        if (_sparse[position]) {
          std::destroy(_sparse[position], _sparse[position] + sparse_page_v);
          alloc_traits::deallocate(_reserved.first, _sparse[position], sparse_page_v);
        }

        std::destroy_at(std::addressof(_sparse[position]));
      }

      auto allocator_ptr = alloc_ptr{_reserved.first};
      alloc_ptr_traits::deallocate(allocator_ptr, _sparse, _bucket);
    }
  }

  std::pair<alloc, size_type> _reserved{};
  alloc_ptr_pointer _sparse{};
  alloc_pointer _packed{};
  size_type _bucket{};
  size_type _count{};
  entity_type _free_list{};
  
}; // class sparse_set

} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
