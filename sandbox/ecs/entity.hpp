#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <limits>

#include <types/primitives.hpp>

namespace sbx {

class entity {

  friend class registry;
  friend bool operator==(const entity& lhs, const entity& rhs) noexcept;

  using id_type = uint16;
  using version_type = uint16;

  inline static constexpr auto invalid_id = std::numeric_limits<id_type>::max();
  inline static constexpr auto invalid_version = std::numeric_limits<version_type>::max();

public:

  static const entity null;

  entity(const entity& other) noexcept = default;

  entity(entity&& other) noexcept;

  ~entity() noexcept = default;

  entity& operator=(const entity& other) noexcept = default;

  entity& operator=(entity&& other) noexcept;

private:

  entity(const id_type id, const version_type version) noexcept;

  void _increment_version() noexcept;

  id_type _id{};
  version_type _version{};

}; // class entity

[[nodiscard]] bool operator==(const entity& lhs, const entity& rhs) noexcept;

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
