#ifndef SBX_ECS_STORAGE_HPP_
#define SBX_ECS_STORAGE_HPP_

#include <type_traits>
#include <memory>

#include <util/type_traits.hpp>

#include "component.hpp"
#include "entity.hpp"
#include "sparse_set.hpp"

namespace sbx {

template<typename Entity, typename Type, typename Allocator = std::allocator<Type>>
class basic_storage : public basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>> {
  static constexpr auto packed_page_v = 1024u;

  using allocator_traits = std::allocator_traits<Allocator>;

  using alloc = typename allocator_traits::template rebind_alloc<Type>;
  using alloc_traits = typename std::allocator_traits<alloc>;
  using alloc_const_pointer = typename alloc_traits::const_pointer;
  using alloc_pointer = typename alloc_traits::pointer;

  using alloc_ptr = typename allocator_traits::template rebind_alloc<alloc_pointer>;
  using alloc_ptr_traits = typename std::allocator_traits<alloc_ptr>;
  using alloc_ptr_const_pointer = typename allocator_traits::template rebind_traits<alloc_const_pointer>::const_pointer;
  using alloc_ptr_pointer = typename alloc_ptr_traits::pointer;

  using entity_traits = sbx::entity_traits<Entity>;
  using component_traits = sbx::component_traits<Type>;
  using underlying_type = basic_sparse_set<Entity, typename allocator_traits::template rebind_alloc<Entity>>;

public:
  using base_type = underlying_type;
  using allocator_type = Allocator;
  using value_type = Type;
  using entity_type = Entity;
  using size_type = std::size_t;
  using pointer = alloc_ptr_pointer;
  using const_pointer = alloc_ptr_const_pointer;

  basic_storage()
  : base_type{},
    _bucket{allocator_type{}, size_type{}},
    _packed{} {
    
  }

  ~basic_storage() override {
    _release_memory();
  }

  void reserve(const size_type capacity) override {
    base_type::reserve(capacity);

    if(capacity > base_type::size()) {
      _assure_at_least(capacity - 1u);
    }
  }

  [[nodiscard]] size_type capacity() const noexcept override {
    return _bucket.second * packed_page_v;
  }

  void shrink_to_fit() override {
    base_type::shrink_to_fit();
    _release_unused_pages();
  }

  [[nodiscard]] const_pointer data() const noexcept {
    return _packed;
  }

  [[nodiscard]] pointer data() noexcept {
    return _packed;
  }

  [[nodiscard]] const value_type& get(const entity_type entity) const noexcept {
    return _element_at(base_type::index(entity));
  }

  [[nodiscard]] value_type& get(const entity_type entity) noexcept {
    return const_cast<value_type&>(std::as_const(*this).get(entity));
  }

  template<typename... Args>
  value_type & emplace(const entity_type entity, Args &&... args) {
    const auto position = base_type::slot();
    auto element = alloc_pointer{_assure_at_least(position)};
    _construct(element, std::forward<Args>(args)...);

    try {
      base_type::try_emplace(entity);
      assert(position == base_type::index(entity));
    } catch (...) {
      std::destroy_at(std::addressof(_element_at(position)));
      throw;
    }

    return *element;
  }

protected:
  void swap_at(const std::size_t lhs, const std::size_t rhs) final {
    std::swap(_element_at(lhs), _element_at(rhs));
  }

  void move_and_pop(const std::size_t from, const std::size_t to) final {
    auto& element = _element_at(from);
    _construct(_assure_at_least(to), std::move(element));
    std::destroy_at(std::addressof(element));
  }

  void swap_and_pop(const Entity entity) override {
    const auto position = base_type::index(entity);
    const auto last = base_type::size() - 1u;

    auto& target = _element_at(position);
    auto& element = _element_at(last);

    // support for nosy destructors
    [[maybe_unused]] auto unused = std::move(target);
    target = std::move(element);
    std::destroy_at(std::addressof(element));

    base_type::swap_and_pop(entity);
  }

  void try_emplace([[maybe_unused]] const Entity entity) override {
    if constexpr(std::is_default_constructible_v<value_type>) {
      const auto position = base_type::slot();
      _construct(_assure_at_least(position));

      try {
        base_type::try_emplace(entity);
        assert(position == base_type::index(entity));
      } catch (...) {
        std::destroy_at(std::addressof(_element_at(position)));
        throw;
      }
    }
  }

private:
  [[nodiscard]] auto& _element_at(const std::size_t position) const {
    return _packed[position / packed_page_v][fast_mod<packed_page_v>(position)];
  }

  auto _assure_at_least(const std::size_t position) {
    const auto idx = position / packed_page_v;

    if(!(idx < _bucket.second)) {
      const size_type size = idx + 1u;
      alloc_ptr allocator_ptr{_bucket.first};
      const auto memory = alloc_ptr_traits::allocate(allocator_ptr, size);
      
      std::uninitialized_value_construct(memory + _bucket.second, memory + size);

      try {
        for(auto next = _bucket.second; next < size; ++next) {
          memory[next] = alloc_traits::allocate(_bucket.first, packed_page_v);
        }
      } catch(...) {
        for(auto next = _bucket.second; next < size && memory[next]; ++next) {
          alloc_traits::deallocate(_bucket.first, memory[next], packed_page_v);
        }

        std::destroy(memory + _bucket.second, memory + size);
        alloc_ptr_traits::deallocate(allocator_ptr, memory, size);
        throw;
      }

      if(_packed) {
        std::uninitialized_copy(_packed, _packed + _bucket.second, memory);
        std::destroy(_packed, _packed + _bucket.second);
        alloc_ptr_traits::deallocate(allocator_ptr, _packed, _bucket.second);
      }

      _packed = memory;
      _bucket.second = size;
    }

    return _packed[idx] + fast_mod<packed_page_v>(position);
  }

  void _release_unused_pages() {
    if(const auto length = base_type::size() / packed_page_v; length < _bucket.second) {
      auto allocator_ptr = alloc_ptr{_bucket.first};
      const auto memory = alloc_ptr_traits::allocate(allocator_ptr, length);
      std::uninitialized_copy(_packed, _packed + length, memory);

      for(auto position = length, last = _bucket.second; position < last; ++position) {
        alloc_traits::deallocate(_bucket.first, _packed[position], packed_page_v);
      }

      std::destroy(_packed, _packed + _bucket.second);
      alloc_ptr_traits::deallocate(allocator_ptr, _packed, _bucket.second);

      _packed = memory;
      _bucket.second = length;
    }
  }

  void _release_memory() {
    if(_packed) {
      if constexpr(component_traits::in_place_delete::value) {
        // no-throw stable erase iteration
        base_type::clear();
      } else {
        for(auto position = size_type{}, last = base_type::size(); position < last; ++position) {
          std::destroy_at(std::addressof(_element_at(position)));
        }
      }

      for(auto position = size_type{}, last = _bucket.second; position < last; ++position) {
        alloc_traits::deallocate(_bucket.first, _packed[position], packed_page_v);
        std::destroy_at(std::addressof(_packed[position]));
      }

      auto allocator_ptr = alloc_ptr{_bucket.first};
      alloc_ptr_traits::deallocate(allocator_ptr, _packed, _bucket.second);
    }
  }

  template<typename... Args>
  void _construct(alloc_pointer ptr, Args &&... args) {
    if constexpr(std::is_aggregate_v<value_type>) {
      alloc_traits::construct(_bucket.first, to_address(ptr), Type{std::forward<Args>(args)...});
    } else {
      alloc_traits::construct(_bucket.first, to_address(ptr), std::forward<Args>(args)...);
    }
  }

  std::pair<alloc, size_type> _bucket;
  alloc_ptr_pointer _packed;

}; // class basic_storage

} // namespace sbx

#endif // SBX_ECS_STORAGE_HPP_
