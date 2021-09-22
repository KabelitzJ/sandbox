#ifndef SBX_ECS_ENTITY_HPP_
#define SBX_ECS_ENTITY_HPP_

#include <limits>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

// (TODO) KAJ 22.09.2021 19.01 Find concept for entity representation
// class entity {

// public:
//   explicit entity(uint32 id);
//   entity(uint16 index, uint16 version);
//   ~entity() = default;

//   uint32 id() const;
//   uint16 index() const;
//   uint16 version() const;

//   operator uint32() const;

// private:
//   uint32 _id;

// }; // class entity

/**
 * @brief 
 */
enum class entity : uint32 {};

} // namespace sbx

#endif // SBX_ECS_ENTITY_HPP_
