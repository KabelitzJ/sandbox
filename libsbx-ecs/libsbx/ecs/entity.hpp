#ifndef LIBSBX_ENTITY_HPP_
#define LIBSBX_ENTITY_HPP_

#include <cinttypes>
#include <memory>
#include <type_traits>
#include <utility>

#include <libsbx/memory/concepts.hpp>

namespace sbx::ecs {

/**
 * @brief Primary template
 *
 * @tparam Type Type of the entity
 */
template<typename Type>
struct basic_entity_traits;

/** @brief Entity traits for 32 bit entity representation */
template<>
struct basic_entity_traits<std::uint32_t> {
  using entity_type = std::uint32_t;

  using id_type = std::uint32_t;
  using version_type = std::uint16_t;

  inline static constexpr auto id_mask_v = id_type{0xFFFFF};
  inline static constexpr auto version_mask_v = id_type{0xFFF};
};

/** @brief Entity traits for 64 bit entity representation */
template<>
struct basic_entity_traits<std::uint64_t> {
  using entity_type = std::uint64_t;

  using id_type = std::uint64_t;
  using version_type = std::uint32_t;

  inline static constexpr auto id_mask_v = id_type{0xFFFFFFFF};
  inline static constexpr auto version_mask_v = id_type{0xFFFFFFFF};
};

/**
 * @brief Entity traits for an enum entity representation. Propagates to the underlying type
 *
 * @tparam Type Enumeration type
 */
template<typename Type>
requires (std::is_enum_v<Type>)
struct basic_entity_traits<Type> : basic_entity_traits<std::underlying_type_t<Type>> {
  using entity_type = Type;
};

/**
 * @brief Entity traits
 *
 * @tparam Type Unsigned integral, enumeration or class type
 */
template<typename Type>
struct entity_traits : basic_entity_traits<Type> {
  using entity_type = basic_entity_traits<Type>::entity_type;
  using id_type = basic_entity_traits<Type>::id_type;
  using version_type = basic_entity_traits<Type>::version_type;

  inline static constexpr auto id_mask_v = basic_entity_traits<Type>::id_mask_v;
  inline static constexpr auto version_mask_v = basic_entity_traits<Type>::version_mask_v;
  inline static constexpr auto version_shift_v = std::popcount(id_mask_v);

  /**
   * @brief Converts the type to its underlying type
   *
   * @param value
   *
   * @return
   */
  static constexpr auto to_underlying(const entity_type value) noexcept -> id_type {
    return static_cast<id_type>(value);
  }

  /**
   * @brief Gets the id part of the entity
   *
   * @param value
   *
   * @return
   */
  static constexpr auto to_id(const entity_type value) noexcept -> id_type {
    return (to_underlying(value) & id_mask_v);
  }

  /**
   * @brief Gets the version part of the entity
   *
   * @param value
   *
   * @return
   */
  static constexpr auto to_version(const entity_type value) noexcept -> version_type {
    return (to_underlying(value) >> version_shift_v);
  }

  /**
   * @brief Constructs a new entity form an id and a version
   *
   * @param id The id part of the entity
   * @param version The version part of the entity
   *
   * @return
   */
  static constexpr auto construct(const id_type id, const version_type version = version_type{0}) noexcept -> entity_type {
    return entity_type{(id & id_mask_v) | (static_cast<id_type>(version) << version_shift_v)};
  }

  /**
   * @brief Gets the next version of an entity
   *
   * @param value
   *
   * @return
   */
  static constexpr auto next(const entity_type value) noexcept -> entity_type {
    const auto version = to_version(value) + 1;
    return construct(to_id(value), static_cast<version_type>(version + (version == version_mask_v)));
  }
}; // struct entity_traits

enum class entity : std::uint32_t { };

struct null_entity_t {

  template<typename Entity>
  [[nodiscard]] constexpr operator Entity() const noexcept {
    return entity_traits<Entity>::construct(entity_traits<Entity>::id_mask_v, entity_traits<Entity>::version_mask_v);
  }

  [[nodiscard]] constexpr auto operator==([[maybe_unused]] const null_entity_t other) const noexcept -> bool {
    return true;
  }

  template<typename Entity>
  [[nodiscard]] constexpr bool operator==(const Entity entity) const noexcept {
    return entity_traits<Entity>::to_id(entity) == entity_traits<Entity>::to_id(*this);
  }

}; // struct null_entity

inline constexpr auto null_entity = null_entity_t{};

} // namespace sbx::ecs

#endif // LIBSBX_ENTITY_HPP_
