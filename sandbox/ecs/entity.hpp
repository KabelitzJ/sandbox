#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <limits>

#include <types/primitives.hpp>

namespace sbx {

/** @brief Represents an entity in the ECS. */
class entity {

  friend class registry;
  friend bool operator==(const entity& lhs, const entity& rhs) noexcept;

  using id_type = uint16;
  using version_type = uint16;
  using value_type = uint32;

  inline static constexpr auto id_mask = id_type{0xFFFF};
  inline static constexpr auto version_mask = version_type{0xFFFF};
  inline static constexpr auto id_shift = std::size_t{16};

public:

  /** @brief The null representation of an entity */
  static const entity null;

  /** @brief Construct a new invalid entity */
  entity() noexcept;

  /**
   * @brief Copy-constructs an entity from another entity
   * 
   * @param other The entity to copy from
   */
  entity(const entity& other) noexcept = default;

  /**
   * @brief Move-constructs an entity from another entity
   * 
   * @param other The entity to move from
   */
  entity(entity&& other) noexcept;

  /** @brief Destroy the entity object */
  ~entity() noexcept = default;

  /**
   * @brief Copy-assigns an entity from another entity
   * 
   * @param other The entity to copy from
   * 
   * @return entity& A reference to this entity
   */
  entity& operator=(const entity& other) noexcept = default;

  /**
   * @brief Move-assigns an entity from another entity
   * 
   * @param other The entity to move from
   * 
   * @return entity& A reference to this entity 
   */
  entity& operator=(entity&& other) noexcept;

private:

  /** 
   * @brief Constructs an entity from an id and a version.
   * 
   * @param id The id of the entity.
   * @param version The version of the entity.
   */
  entity(const id_type id, const version_type version) noexcept;

  /**
   * @brief Increments the version of the entity.
   * 
   * @warning The version rolls over to 0 when it reaches the maximum value.
   */
  void _increment_version() noexcept;

  /**
   * @brief Retrieves the id of the entity.
   * 
   * @return id_type The id of the entity.
   */
  [[nodiscard]] id_type _id() const noexcept;

  /**
   * @brief Retrieves the version of the entity.
   * 
   * @return version_type The version of the entity.
   */
  [[nodiscard]] version_type _version() const noexcept;

  value_type _value{};

}; // class entity

[[nodiscard]] bool operator==(const entity& lhs, const entity& rhs) noexcept;

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
