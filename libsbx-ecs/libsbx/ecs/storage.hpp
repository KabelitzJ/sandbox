#ifndef LIBSBX_STORAGE_HPP_
#define LIBSBX_STORAGE_HPP_

#include <type_traits>
#include <iostream>
#include <limits>
#include <vector>
#include <memory>

#include <libsbx/memory/concepts.hpp>
#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/ecs/sparse_set.hpp>
#include <libsbx/ecs/component.hpp>

#include <libsbx/ecs/detail/storage_iterator.hpp>

namespace sbx::ecs {

template<typename Type, typename Entity, memory::allocator_for<Type> Allocator = std::allocator<Type>>
class basic_storage : public basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>> {

  using allocator_traits = std::allocator_traits<Allocator>;
  using container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::template rebind_alloc<typename allocator_traits::pointer>>;
  using underlying_type = basic_sparse_set<Entity, typename allocator_traits::template rebind_alloc<Entity>>;
  using underlying_iterator = typename underlying_type::basic_iterator;
  using component_traits = component_traits<Type, Entity>;

public:

  using allocator_type = Allocator;
  using base_type = underlying_type;
  using value_type = Type;
  using entity_type = Entity;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = typename container_type::pointer;
  using const_pointer = typename allocator_traits::template rebind_traits<typename allocator_traits::const_pointer>::const_pointer;
  using iterator = detail::storage_iterator<container_type, component_traits::page_size>;
  using const_iterator = detail::storage_iterator<const container_type, component_traits::page_size>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static constexpr auto storage_policy = static_cast<deletion_policy>(component_traits::in_place_delete);

  basic_storage()
  : basic_storage{allocator_type{}} { }

  explicit basic_storage(const allocator_type &allocator)
  : base_type{storage_policy, allocator},
    _container{allocator} { }

  basic_storage(const basic_storage& other) = delete;

  basic_storage(basic_storage&& other) noexcept
  : base_type{std::move(other)},
    _container{std::move(other._container)} { }

  ~basic_storage() override {
    _shrink_to_size(0u);
  }

  auto operator=(const basic_storage& other) -> basic_storage& = delete;

  auto operator=(basic_storage&& other) noexcept -> basic_storage& {
    utility::assert_that(allocator_traits::is_always_equal::value || get_allocator() == other.get_allocator(), "Copying a storage is not allowed");
    swap(other);
    return *this;
  }

  auto swap(basic_storage& other) noexcept -> void {
    using std::swap;
    swap(_container, other._container);
    base_type::swap(other);
  }

  constexpr auto get_allocator() const noexcept -> allocator_type {
    return _container.get_allocator();
  }

  auto reserve(const size_type capacity) -> void override {
    if(capacity != 0u) {
      base_type::reserve(capacity);
      _assure_at_least(capacity - 1u);
    }
  }

  [[nodiscard]] auto get(const entity_type entity) const noexcept -> const value_type& {
    return element_at(base_type::index(entity));
  }

  [[nodiscard]] auto get(const entity_type entity) noexcept -> value_type& {
    return const_cast<value_type&>(std::as_const(*this).get(entity));
  }

  [[nodiscard]] std::tuple<const value_type&> get_as_tuple(const entity_type entity) const noexcept {
    return std::forward_as_tuple(get(entity));
  }

  [[nodiscard]] std::tuple<value_type&> get_as_tuple(const entity_type entity) noexcept {
    return std::forward_as_tuple(get(entity));
  }

protected:

  auto pop(underlying_iterator first, underlying_iterator last) -> void override {
    for(allocator_type allocator{get_allocator()}; first != last; ++first) {
      auto& element = _element_at(base_type::index(*first));

      if constexpr(component_traits::in_place_delete) {
        base_type::in_place_pop(first);
        allocator_traits::destroy(allocator, std::addressof(element));
      } else {
        auto& other = element_at(base_type::size() - 1u);
        [[maybe_unused]] auto unused = std::exchange(element, std::move(other));
        allocator_traits::destroy(allocator, std::addressof(other));
        base_type::swap_and_pop(first);
      }
    }
  }

  auto pop_all() -> void override {
    auto allocator = get_allocator();

    for(auto first = base_type::begin(); !(first.index() < 0); ++first) {
      if constexpr(component_traits::in_place_delete) {
        if(*first != tombstone_entity) {
          base_type::in_place_pop(first);
          allocator_traits::destroy(allocator, std::addressof(_element_at(static_cast<size_type>(first.index()))));
        }
      } else {
        base_type::swap_and_pop(first);
        allocator_traits::destroy(allocator, std::addressof(_element_at(static_cast<size_type>(first.index()))));
      }
    }
  }

  auto try_emplace([[maybe_unused]] const entity_type entity, [[maybe_unused]] const bool force_back, const void* value) -> underlying_iterator override {
    if(value != nullptr) {
      if constexpr(std::is_copy_constructible_v<value_type>) {
        return _emplace_element(entity, force_back, *static_cast<const value_type *>(value));
      } else {
        return base_type::end();
      }
    } else {
      if constexpr(std::is_default_constructible_v<value_type>) {
        return _emplace_element(entity, force_back);
      } else {
        return base_type::end();
      }
    }
  }

private:

  auto _element_at(const std::size_t position) const {
    return _container[position / component_traits::page_size][utility::fast_mod(position, component_traits::page_size)];
  }

  auto _assure_at_least(const std::size_t position) {
    const auto index = position / component_traits::age_size;

    if(index >= _container.size()) {
      auto current = _container.size();
      auto allocator = get_allocator();
      _container.resize(index + 1u, nullptr);

      try {
        for(const auto last = _container.size(); current < last; ++current) {
          _container[current] = alloc_traits::allocate(allocator, component_traits::page_size);
        }
      } catch (...) {
        _container.resize(current);
        throw;
      }
    }

    return _container[index] + utility::fast_mod(position, component_traits::page_size);
  }

  template<typename... Args>
  auto _emplace_element(const entity_type entity, const bool force_back, Args &&...args) {
    const auto iterator = base_type::try_emplace(entity, force_back);

    try {
      auto* element = std::to_address(assure_at_least(static_cast<size_type>(iterator.index())));
      std::uninitialized_construct_using_allocator(element, get_allocator(), std::forward<Args>(args)...);
    } catch {
      base_type::pop(iterator, iterator + 1u);
      throw;
    }

    return iterator;
  }

  auto _shrink_to_size(const size_type size) -> void {
    const auto from = (size + component_traits::page_size - 1u) / component_traits::page_size;
    auto allocator = get_allocator();

    for(auto position = size, length = base_type::size(); position < length; ++position) {
      if constexpr(component_traits::in_place_delete) {
        if(base_type::data()[position] != tombstone_entity) {
          allocator_traits::destroy(allocator, std::addressof(_element_at(position)));
        }
      } else {
        allocator_traits::destroy(allocator, std::addressof(_element_at(position)));
      }
    }

    for(auto position = from, last = _container.size(); position < last; ++position) {
      allocator_traits::deallocate(allocator, _container[position], component_traits::page_size);
    }

    _container.resize(from);
    _container.shrink_to_fit();
  }

  void _swap_at(const size_type lhs, const size_type rhs) {
    using std::swap;
    swap(_element_at(lhs), _element_at(rhs));
  }

  void _move_to(const size_type lhs, const size_type rhs) {
    auto& element = _element_at(lhs);
    auto allocator = get_allocator();
    std::uninitialized_construct_using_allocator(std::to_address(_assure_at_least(rhs)), allocator, std::move(element));
    allocator_traits::destroy(allocator, std::addressof(element));
  }

  container_type _container;

}; // class basic_storage

} // namespace sbx::ecs

#endif // LIBSBX_STORAGE_HPP_
