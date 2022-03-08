#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <bitset>
#include <iostream>

#include <types/primitives.hpp>

namespace sbx {

class entity {

  friend class registry;
  friend class sparse_set;
  friend bool operator==(const entity& lhs, const entity& rhs) noexcept;

  using id_type = uint32;
  using version_type = uint16;
  using value_type = uint32;

  inline static constexpr auto id_mask = value_type{0xFFFFF};
  inline static constexpr auto version_mask = value_type{0xFFF};
  inline static constexpr auto version_shift = std::size_t{12};

public:

  static const entity null;

  entity(const entity& other) noexcept = default;

  entity(entity&& other) noexcept;

  ~entity() noexcept = default;

  entity& operator=(const entity& other) noexcept = default;

  entity& operator=(entity&& other) noexcept;

  void print() {
    const auto binary = std::bitset<32>{_value};
    std::cout << binary << std::endl;
  }

private:

  entity() noexcept;

  entity(const id_type id, const version_type version) noexcept;

  id_type _id() const noexcept;

  version_type _version() const noexcept;

  void _incement_version() noexcept;

  value_type _value{};

}; // class entity

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
