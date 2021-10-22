#ifndef SBX_ECS_VIEW_HPP_
#define SBX_ECS_VIEW_HPP_

#include <array>
#include <tuple>
#include <algorithm>

#include <types/type_traits.hpp>

#include "entity.hpp"
#include "storage.hpp"
#include "component.hpp"

namespace sbx {

template<typename... Type>
struct exclude_t : type_list<Type...> {};

template<typename... Type>
inline constexpr exclude_t<Type...> exclude{};


template<typename... Type>
struct get_t : type_list<Type...> {};

template<typename... Type>
inline constexpr get_t<Type...> get{};


// [NOTE] KAJ 2021-10-22 14:21: Maybe unused
// template<typename, typename, typename, typename>
// class basic_view;

template<typename, typename, typename, typename = void>
class basic_view;

template<typename Get, typename Exclude = exclude_t<>>
using view = basic_view<entity, Get, Exclude>;


/**
 * @brief Multi component view into a registry
 *
 * @tparam Entity Entity type of the registry
 * @tparam Components List of component types that should be in the view
 * @tparam Excludes List of component types that should not be in the view
 */
template<typename Entity, typename... Components, typename... Excludes>
class basic_view<Entity, get_t<Components...>, exclude_t<Excludes...>> {

  static constexpr auto is_multi_type_v = (sizeof...(Components) + sizeof...(Excludes)) != 1u;

  using basic_common_type = std::common_type_t<typename storage_traits<Entity, std::remove_const_t<Components>>::storage_type::base_type...>;

public:

  using entity_type = Entity;
  using size_type = std::size_t;
  // using iterator = internal::view_iterator<basic_common_type, typename basic_common_type::iterator, sizeof...(Components) - 1u, sizeof...(Excludes)>;
  // using reverse_iterator = internal::view_iterator<basic_common_type, typename basic_common_type::reverse_iterator, sizeof...(Components) - 1u, sizeof...(Excludes)>;
  // using iterable_view = iterable;

  template<typename Component>
  using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

  basic_view() noexcept
  : _pools{},
    _filter{},
    _view{} { }

  basic_view(storage_type<Components>&... components, const storage_type<Excludes>&... excludes) ENTT_NOEXCEPT
  : _pools{&components...},
    _filter{&excludes...},
    _view{_candidate()} { }

  basic_view(const basic_view&) = delete;

  basic_view(basic_view&&) = default;

  basic_view& operator=(const basic_view&) = delete;

  basic_view& operator=(basic_view&&) = default;

private:

  [[nodiscard]] const auto* _candidate() const noexcept {
    return std::min({static_cast<const basic_common_type*>(std::get<storage_type<Components>*>(_pools))...}, [](const auto* lhs, const auto* rhs) {
      return lhs->size() < rhs->size();
    });
  }

  template<typename, typename, typename, typename>
  friend class basic_view;

  std::tuple<storage_type<Components>*...> _pools{};
  std::array<const basic_common_type*, sizeof...(Excludes)> _filter{};
  const basic_common_type* _view{};

}; // class basic_view


/**
 * @brief Single component view into a registry
 *
 * @tparam Entity Entity type of the registry
 * @tparam Component component type that should be in the view
 */
template<typename Entity, typename Component>
class basic_view<Entity, get_t<Component>, exclude_t<>, std::void_t<std::enable_if_t<!in_place_delete_v<std::remove_const_t<Component>>>>> {

  using basic_common_type = typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type::base_type;

public:

  using entity_type = Entity;
  using size_type = std::size_t;
  using iterator = typename basic_common_type::iterator;
  using reverse_iterator = typename basic_common_type::reverse_iterator;
  // using iterable_view = internal::iterable_storage<Entity, Component>;
  using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

  basic_view() noexcept
  : _pools{},
    _filter{} { }

  basic_view(storage_type& pools) noexcept
  : _pools{&pools},
    _filter{} { }

  basic_view(const basic_view&) = delete;

  basic_view(basic_view&&) = default;

  basic_view& operator=(const basic_view&) = delete;

  basic_view& operator=(basic_view&&) = default;

private:

  template<typename, typename, typename, typename>
  friend class basic_view;

  std::tuple<storage_type*> _pools{};
  std::tuple<> _filter{};

}; // class basic_view


/**
 * @brief Deduction guide.
 */
template<typename... Storage>
basic_view(Storage&... storage) -> basic_view<std::common_type_t<typename Storage::entity_type...>, get_t<constness_as_t<typename Storage::value_type, Storage>...>, exclude_t<>>;


} // namespace sbx

#endif // SBX_ECS_VIEW_HPP_
