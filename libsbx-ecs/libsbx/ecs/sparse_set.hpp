#ifndef LIBSBX_SPARSE_SET_HPP_
#define LIBSBX_SPARSE_SET_HPP_

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <utility>

#include <libsbx/utility/assert.hpp>
#include <libsbx/utility/fast_mod.hpp>

#include <libsbx/memory/concepts.hpp>

namespace sbx::ecs {

enum class deletion_policy : std::uint8_t {
  swap_and_pop = 0u,
  in_place = 1u,
  swap_only = 2u,
  unspecified = swap_and_pop
}; // enum class deletion_policy

template<typename Entity, memory::allocator_for<Entity> Allocator = std::allocator<Entity>>
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using entity_traits = entity_traits<Entity>;

  using dense_storage_type = std::vector<Entity, Allocator>;
  using sparse_storage_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<allocator_traits::pointer>>;

  inline static constexpr auto max_size = static_cast<std::size_t>(entity_traits::to_id(null_entity));

public:

  using size_type = std::size_t;
  using allocator_type = Allocator;
  using entity_type = Entity;
  using pointer = entity_type*;
  using const_pointer = const entity_type*;
  using reference = entity_type&;
  using const_reference = const entity_type&;

  basic_sparse_set(const deletion_policy policy = deletion_policy::unspecified, const allocator_type& allocator = allocator_type{})
  : _dense{allocator},
    _sparse{allocator},
    _policy{policy},
    _head{_policy_to_head()} {
    utility::assert_that(entity_traits::version_mask || _policy != deletion_policy::in_place, "Policy does not support zero-sized versions");
  }

  basic_sparse_set(const basic_sparse_set&) = delete;

  basic_sparse_set(basic_sparse_set&& other) noexcept
  : _dense{std::move(other._dense)},
    _sparse{std::move(other._sparse)},
    _policy{other._policy},
    _head{std::exchange(other._head, _policy_to_head())} { }

    basic_sparse_set(basic_sparse_set&& other, const allocator_type& allocator)
  : _dense{std::move(other._dense), allocator},
    _sparse{std::move(other._sparse), allocator},
    _policy{other._policy},
    _head{std::exchange(other._head, _policy_to_head())} { 
    utility::assert_that(allocator_type::is_always_equal::value || get_allocator() == other.get_allocator(), "Copying a sparse set is not allowed");
  }

  virtual ~basic_sparse_set() {
    _release_sparse_pages();
  }

  auto operator=(const basic_sparse_set& other) -> basic_sparse_set& = delete;

  auto operator=(basic_sparse_set&& other) noexcept -> basic_sparse_set& {
    utility::assert_that(allocator_type::is_always_equal::value || get_allocator() == other.get_allocator(), "Copying a sparse set is not allowed");

    swap(other);

    return *this;
  }

  auto swap(basic_sparse_set& other) noexcept -> void {
    using std::swap;

    swap(_sparse, other._sparse);
    swap(_dense, other._dense);
    swap(_policy, other._policy);
    swap(_head, other._head);
  }

  [[nodiscard]] constexpr auto get_allocator() const noexcept -> allocator_type {
    return _dense.get_allocator();
  }

  [[nodiscard]] auto policy() const noexcept -> deletion_policy {
    return _policy;
  }

  [[nodiscard]] auto free_list() const noexcept -> size_type {
    return _head;
  }

  virtual void reserve(const size_type capacity) {
    _dense.reserve(capacity);
  }

  [[nodiscard]] virtual auto capacity() const noexcept -> size_type {
    return _dense.capacity();
  }

protected:



private:

  [[nodiscard]] auto _policy_to_head() const noexcept -> size_type {
    return static_cast<size_type>(max_size * static_cast<size_type>(_policy != deletion_policy::swap_only));
  }

  [[nodiscard]] auto _entity_to_position(const entity_type entity) const noexcept {
    return static_cast<size_type>(entity_traits::to_id(entity));
  }

  [[nodiscard]] auto _position_to_page(const size_type position) const noexcept {
    return static_cast<size_type>(position / entity_traits::page_size);
  }

  [[nodiscard]] auto _sparse_pointer(const entity_type entity) const -> pointer {
    const auto position = _entity_to_position(entity);
    const auto page = _position_to_page(position);

    return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + utility::fast_mod<entity_traits::page_size>(position)) : nullptr;
  }

  [[nodiscard]] auto _sparse_reference(const entity_type entity) const -> reference {
    utility::assert_that(_sparse_pointer(entity), "Invalid element");

    const auto position = _entity_to_position(entity);
    const auto page = _position_to_page(position);

    return _sparse[page][utility::fast_mod<entity_traits::page_size>(position)];
  }

  void _release_sparse_pages() {
    auto& page_allocator = _dense.get_allocator();

    for(auto&& page : _sparse) {
      if(page != nullptr) {
        std::destroy(page, page + entity_traits::page_size);
        allocator_traits::deallocate(page_allocator, page, entity_traits::page_size);
        page = nullptr;
      }
    }
  }

  [[nodiscard]] auto _assure_at_least(const entity_type entity) -> reference {
    const auto position = _entity_to_position(entity);
    const auto page = _position_to_page(position);

    if(page >= _sparse.size()) {
      _sparse.resize(page + 1u, nullptr);
    }

    if(!_sparse[page]) {
      constexpr auto init = null_entity;
      auto& page_allocator = _dense.get_allocator();

      _sparse[page] = allocator_traits::allocate(page_allocator, entity_traits::page_size);

      std::uninitialized_fill(_sparse[page], _sparse[page] + entity_traits::page_size, init);
    }

    return _sparse[page][utility::fast_mod<entity_traits::page_size>(position)];
  }

  dense_storage_type _dense;
  sparse_storage_type _sparse;
  deletion_policy _policy;
  size_type _head;

}; // class sparse_set

} // namespace sbx::ecs

#endif // LIBSBX_SPARSE_SET_HPP_
