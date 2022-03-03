#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <limits>

#include <types/primitives.hpp>

namespace sbx {

class entity {

  friend int main();
  friend int main();
  friend class registry;
  friend bool operator==(const entity& lhs, const entity& rhs) noexcept;

  using id_type = uint16;
  using version_type = uint16;
  using value_type = uint32;

  inline static constexpr auto id_mask = id_type{0xFFFF};
  inline static constexpr auto version_mask = version_type{0xFFFF};
  inline static constexpr auto id_shift = std::size_t{16};

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

  [[nodiscard]] id_type _id() const noexcept;

  [[nodiscard]] version_type _version() const noexcept;

  value_type _value{};

}; // class entity

[[nodiscard]] bool operator==(const entity& lhs, const entity& rhs) noexcept;

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
