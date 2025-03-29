#ifndef LIBSBX_REGISTRY_HPP_
#define LIBSBX_REGISTRY_HPP_

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <optional>
#include <algorithm>
#include <tuple>
#include <ranges>

#include <libsbx/memory/concepts.hpp>
#include <libsbx/memory/observer_ptr.hpp>
#include <libsbx/memory/dense_map.hpp>

#include <libsbx/ecs/entity.hpp>
#include <libsbx/ecs/sparse_set.hpp>
#include <libsbx/ecs/storage.hpp>
#include <libsbx/ecs/view.hpp>

namespace sbx::ecs {

namespace detail {

struct id_generator final {
  [[nodiscard]] static auto next() noexcept -> std::uint32_t {
    static auto id = std::uint32_t{};
    return id++;
  }
}; // struct type_index

} // namespace detail

template<typename Type>
struct type_index {

  [[nodiscard]] static auto value() noexcept -> std::uint32_t {
    static const auto value = detail::id_generator::next();
    return value;
  }

  [[nodiscard]] constexpr operator std::uint32_t() const noexcept {
    return value();
  }

}; // struct type_index

template<typename Type, typename Entity = entity, memory::allocator_for<Type> Allocator = std::allocator<Type>>
struct storage_type {
  using type = basic_storage<Type, Entity, Allocator>;
}; // struct storage_type

template<typename... Args>
using storage_type_t = typename storage_type<Args...>::type;

template<typename Type, typename Entity = entity, memory::allocator_for<std::remove_const_t<Type>> Allocator = std::allocator<std::remove_const_t<Type>>>
struct storage_for {
  using type = constness_as_t<storage_type_t<std::remove_const_t<Type>, Entity, Allocator>, Type>;
}; // struct storage_for

template<typename... Args>
using storage_for_t = typename storage_for<Args...>::type;

template<typename Entity, memory::allocator_for<Entity> Allocator = std::allocator<Entity>>
class basic_registry {

  using base_type = basic_sparse_set<Entity, Allocator>;
  using allocator_traits = std::allocator_traits<Allocator>;

  using pool_container_type = memory::dense_map<std::uint32_t, std::shared_ptr<base_type>, std::identity, std::equal_to<>, memory::rebound_allocator_t<Allocator, std::pair<const std::uint32_t, std::shared_ptr<base_type>>>>;
  using entity_traits = entity_traits<Entity>;

  template<typename Type>
  using storage_for_type = storage_for_t<Type, Entity, memory::rebound_allocator_t<Allocator, std::remove_const_t<Type>>>;

public:

  using allocator_type = Allocator;
  using entity_type = entity_traits::value_type;
  using version_type = entity_traits::version_type;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using common_type = base_type;

  basic_registry()
  : basic_registry{allocator_type{}} { }

  explicit basic_registry(const allocator_type& allocator)
  : basic_registry{0u, allocator} { }

  basic_registry(const size_type count, const allocator_type &allocator = allocator_type{})
  : _pools{allocator},
    _entities{allocator} {
    _pools.reserve(count);
  }

  basic_registry(const basic_registry& other) = delete;

  basic_registry(basic_registry&& other) noexcept
  : _pools{std::move(other._pools)},
    _entities{std::move(other._entities)} { }

  ~basic_registry() = default;

  auto operator=(const basic_registry& other) -> basic_registry& = delete;

  auto operator=(basic_registry&& other) noexcept -> basic_registry& {
    swap(other);
    return *this;
  }

  auto swap(basic_registry& other) noexcept -> void {
    using std::swap;
    swap(_pools, other._pools);
    swap(_entities, other._entities);
  }

  [[nodiscard]] constexpr auto get_allocator() const noexcept -> allocator_type {
    return _entities.get_allocator();
  }

  [[nodiscard]] auto is_valid(const entity_type entity) const -> bool {
    return static_cast<size_type>(_entities.find(entity).index()) < _entities.free_list();
  }

  auto create() -> entity_type {
    return _entities.generate();
  }

  auto destroy(const entity_type entity) -> version_type {
    for (auto position = _pools.size(); position != 0u; --position) {
      _pools.begin()[static_cast<typename pool_container_type::difference_type>(position - 1u)].second->remove(entity);
    }

    _entities.erase(entity);
    return _entities.current(entity);
  }

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, Args...>)
  auto emplace(const entity_type entity, Args&&... args) -> decltype(auto) {
    utility::assert_that(is_valid(entity), "Invalid entity");
    return _assure<Type>().emplace(entity, std::forward<Args>(args)...);
  }

  template<typename Type, typename... Other>
  auto remove(const entity_type entity) -> size_type {
    return (_assure<Type>().remove(entity) + ... + _assure<Other>().remove(entity));
  }

  template<typename... Type>
  [[nodiscard]] auto get([[maybe_unused]] const entity_type entity) const -> decltype(auto) {
    if constexpr (sizeof...(Type) == 1u) {
      return (_assure<std::remove_const_t<Type>>()->get(entity), ...);
    } else {
      return std::forward_as_tuple(get<Type>(entity)...);
    }
  }

  template<typename... Type>
  [[nodiscard]] auto get([[maybe_unused]] const entity_type entity) -> decltype(auto) {
    if constexpr (sizeof...(Type) == 1u) {
      return (static_cast<storage_for_type<Type>&>(_assure<std::remove_const_t<Type>>()).get(entity), ...);
    } else {
      return std::forward_as_tuple(get<Type>(entity)...);
    }
  }

  template<typename Type, typename... Args>
  requires (std::is_constructible_v<Type, Args...>)
  [[nodiscard]] auto get_or_emplace(const entity_type entity, Args&&... args) -> decltype(auto) {
    auto& pool = _assure<Type>();
    utility::assert_that(is_valid(entity), "Invalid entity");
    return pool.contains(entity) ? pool.get(entity) : pool.emplace(entity, std::forward<Args>(args)...);
  }

  template<typename... Type>
  [[nodiscard]] auto try_get([[maybe_unused]] const entity_type entity) const {
    if constexpr (sizeof...(Type) == 1u) {
      const auto* pool = _assure<std::remove_const_t<Type>...>();
      return (pool && pool->contains(entity)) ? std::addressof(pool->get(entity)) : nullptr;
    } else {
      return std::make_tuple(try_get<Type>(entity)...);
    }
  }

  template<typename... Type>
  [[nodiscard]] auto try_get([[maybe_unused]] const entity_type entity) {
    if constexpr (sizeof...(Type) == 1u) {
      return (const_cast<Type*>(std::as_const(*this).template try_get<Type>(entity)), ...);
    } else {
      return std::make_tuple(try_get<Type>(entity)...);
    }
  }

  template<typename... Type>
  void clear() {
    if constexpr (sizeof...(Type) == 0u) {
      for (auto position = _pools.size(); position; --position) {
        _pools.begin()[static_cast<typename pool_container_type::difference_type>(position - 1u)].second->clear();
      }

      const auto element = _entities.each();
      _entities.erase(element.begin().base(), element.end().base());
    } else {
      (_assure<Type>().clear(), ...);
    }
  }

  template<typename Type, typename... Other>
  [[nodiscard]] auto view() const -> basic_view<get_t<storage_for_type<const Type>, storage_for_type<const Other>...>> {
    auto element = basic_view<get_t<storage_for_type<const Type>, storage_for_type<const Other>...>>{};
    [&element](const auto*... current) { ((current ? element.set_storage(*current) : void()), ...); }(_assure<std::remove_const_t<Other>>()..., _assure<std::remove_const_t<Type>>());
    return element;
  }

  template<typename Type, typename... Other>
  [[nodiscard]] auto view() -> basic_view<get_t<storage_for_type<Type>, storage_for_type<Other>...>> {
    return {_assure<std::remove_const_t<Type>>(), _assure<std::remove_const_t<Other>>()...};
  }

private:

  template<typename Type>
  requires (std::is_same_v<Type, std::decay_t<Type>> && !std::is_same_v<Type, entity_type>)
  [[nodiscard]] auto& _assure([[maybe_unused]] const std::uint32_t id = type_index<Type>::value()) {
    using storage_type = storage_for_type<Type>;

    if (auto iterator = _pools.find(id); iterator != _pools.cend()) {
      return static_cast<storage_type&>(*iterator->second);
    }

    using storage_allocator_type = typename storage_type::allocator_type;
    using pool_type = typename pool_container_type::mapped_type;

    auto pool = pool_type{};

    if constexpr (std::is_void_v<Type> && !std::is_constructible_v<storage_allocator_type, allocator_type>) {
      pool = std::allocate_shared<storage_type>(get_allocator(), storage_allocator_type{});
    } else {
      pool = std::allocate_shared<storage_type>(get_allocator(), get_allocator());
    }

    _pools.emplace(id, pool);

    return static_cast<storage_type&>(*pool);
  }

  template<typename Type>
  requires (std::is_same_v<Type, std::decay_t<Type>> && !std::is_same_v<Type, entity_type>)
  [[nodiscard]] const auto* _assure([[maybe_unused]] const std::uint32_t id = type_index<Type>::value()) const {
    if (const auto iterator = _pools.find(id); iterator != _pools.cend()) {
      return static_cast<const storage_for_type<Type>*>(iterator->second.get());
    }

    return static_cast<const storage_for_type<Type>*>(nullptr);
  }

  pool_container_type _pools;
  storage_for_type<entity_type> _entities;

}; // class basic_registry

using registry = basic_registry<entity>;

} // namespace sbx::ecs

#endif // LIBSBX_REGISTRY_HPP_
