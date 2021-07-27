#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <bitset>
#include <cinttypes>

namespace sbx {

class entity {

public:
  entity(std::uint16_t index, std::uint16_t version);
  ~entity() = default;

  std::uint32_t id() const;
  std::uint16_t index() const;
  std::uint16_t version() const;

private:
  std::uint32_t _id;
  std::bitset<32> _component_mask; // TODO: think of something not hard coded

}; // class entity

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
