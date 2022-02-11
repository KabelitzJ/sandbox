#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <concepts>

namespace sbx {

template<std::unsigned_integral Type>
class entity {

public:

  using id_type = Type;

  constexpr entity(const entity<id_type>& other) noexcept = delete;

  constexpr entity(entity<id_type>&& other) noexcept;

  ~entity() noexcept;

  constexpr entity<id_type>& operator=(const entity<id_type>& other) noexcept = delete;

  constexpr entity<id_type>& operator=(entity<id_type>&& other) noexcept;

private:

  constexpr entity(const id_type id) noexcept;

  id_type _id{};

}; // class entity

template<std::unsigned_integral Type>
constexpr bool operator==(const entity<Type>& lhs, const entity<Type>& rhs) noexcept;

} // namespace sbx

#include "entity.inl"

#endif // SBX_ECS_ENTITY_HPP_
