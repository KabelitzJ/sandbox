#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <vector>
#include <utility>
#include <iterator>
#include <memory>
#include <cassert>
#include <type_traits>

#include <types/primitives.hpp>

#include <util/memory.hpp>

#include "entity.hpp"

namespace sbx {

template<typename Entity, typename Allocator = std::allocator<Entity>>
class basic_sparse_set {

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

  basic_sparse_set(const allocator_type& allocator = {})
  : _reserved{allocator, size_type{}},
    _sparse{},
    _packed{},
    _bucket{},
    _count{},
    _free_list{tombstone} {

  }

  virtual ~basic_sparse_set() {
    _release_memory();
  }

  [[nodiscard]] size_type slot() const noexcept {
    return _free_list == null ? _count : static_cast<size_type>(entity_traits::to_entity(_free_list));
  }

  virtual void reserve(const size_type capacity) {
    if(capacity > _reserved.second()) {
      _resize_packed(capacity);
    }
  }

  [[nodiscard]] virtual size_type capacity() const noexcept {
    return _reserved.second();
  }

  virtual void shrink_to_fit() {
    if(_count < _reserved.second()) {
      _resize_packed(_count);
    }
  }

  [[nodiscard]] size_type extent() const noexcept {
    return _bucket * sparse_page_v;
  }

  [[nodiscard]] size_type size() const noexcept {
    return _count;
  }

  [[nodiscard]] bool empty() const noexcept {
    return (_count == size_type{0u});
  }

  [[nodiscard]] pointer data() const noexcept {
    return _packed;
  }

  // [NOTE] KAJ 2021-09-27 22:21: Add iterators here

  [[nodiscard]] bool contains(const entity_type entity) const noexcept {
    if(auto element = _sparse_pointer(entity); element) {
      constexpr auto capacity = entity_traits::to_entity(null);

      return (((~capacity & entity_traits::to_integral(entity)) ^ entity_traits::to_integral(*element)) < capacity);
    }

    return false;
  }

  // [NOTE] KAJ 2021-09-27 22:34: Wait for iterator implementation
  // [[nodiscard]] iterator find(const entity_type entity) const noexcept {
  //   return contains(entity) ? --(end() - index(entity)) : end();
  // }

  [[nodiscard]] version_type current_version(const entity_type entity) const {
    if(auto element = _sparse_pointer(entity); element) {
      return entity_traits::to_version(*element);
    }

    return entity_traits::to_version(tombstone);
  }

  // [NOTE] KAJ 2021-09-27 22:35: Element access here

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

  // [NOTE] KAJ 2021-09-27 22:52: Wait for iterator implementation
  // void clear() {
  //
  // }

protected:

  virtual void swap_at(const std::size_t, const std::size_t) {}

  virtual void move_and_pop(const std::size_t, const std::size_t) {}

  virtual void swap_and_pop(const Entity entity) {
    auto& ref = _sparse_reference(entity);
    const auto position = static_cast<size_type>(entity_traits::to_entity(ref));

    assert(_packed[position] == entity);

    auto& last = _packed[--_count];

    _packed[position] = last;
    auto& elem = _sparse_reference(last);
    elem = entity_traits::combine(entity_traits::to_integral(ref), entity_traits::to_integral(elem));
    
    
    ref = null;
    
    assert((last = tombstone, true));
  }

  virtual void try_emplace(const Entity entity) {
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    const auto page = position / sparse_page_v;

    if(!(page < _bucket)) {
      const auto size = size_type{page + 1u};
      const auto allocator_ptr = alloc_ptr{_reserved.first()};
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
      _sparse[page] = alloc_traits::allocate(_reserved.first(), sparse_page_v);
      std::uninitialized_fill(_sparse[page], _sparse[page] + sparse_page_v, null);
    }

    auto& element = _sparse[page][fast_mod<sparse_page_v>(position)];
    assert(entity_traits::to_version(element) == entity_traits::to_version(tombstone));

    if(_free_list == null) {
      if(_count == _reserved.second()) {
        const auto size = static_cast<size_type>(_reserved.second() * growth_factor_v);
        _resize_packed(size + !(size > _reserved.second()));
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
    assert((request != _reserved.second()) && !(request < _count));

    const auto memory = alloc_traits::allocate(_reserved.first(), request);

    std::uninitialized_fill(memory + _count, memory + request, tombstone);

    if (_packed) {
      std::uninitialized_copy(_packed, _packed + _count, memory);
      std::destroy(_packed, _packed + _reserved.second());
      alloc_traits::deallocate(_reserved.first(), _packed, _reserved.second());
    }

    _packed = memory;
    _reserved.second() = request;
  }

  void _release_memory() {
    if(_packed) {
      std::destroy(_packed, _packed + _reserved.second());
      alloc_traits::deallocate(_reserved.first(), _packed, _reserved.second());
    }

    if (_sparse) {
      for (auto position = size_type{}; position < _bucket; ++position) {
        if (_sparse[position]) {
          std::destroy(_sparse[position], _sparse[position] + sparse_page_v);
          alloc_traits::deallocate(_reserved.first(), _sparse[position], sparse_page_v);
        }

        std::destroy_at(std::addressof(_sparse[position]));
      }

      const auto allocator_ptr = alloc_ptr{_reserved.first()};
      alloc_ptr_traits::deallocate(allocator_ptr, _sparse, _bucket);
    }
  }

  static constexpr auto growth_factor_v = 1.5f;
  static constexpr auto sparse_page_v = 4096u;

  std::pair<alloc, size_type> _reserved{};
  alloc_ptr_pointer _sparse{};
  alloc_pointer _packed{};
  size_type _bucket{};
  size_type _count{};
  entity_type _free_list{};
  

}; // class sparse_set

} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
