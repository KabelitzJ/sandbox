#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <types/primitives.hpp>
#include <types/basic_traits.hpp>

namespace sbx {

class entity {

public:
  explicit entity(uint32 id);
  entity(uint16 index, uint16 version);
  ~entity() = default;

  uint32 id() const;
  uint16 index() const;
  uint16 version() const;

  operator uint32() const;

private:
  uint32 _id;

}; // class entity


template<>
struct is_index<entity> : std::true_type {};

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
