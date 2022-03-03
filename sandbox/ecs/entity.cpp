#include "entity.hpp"

#include <utility>

#include <platform/assert.hpp>

namespace sbx {

const entity entity::null{entity::invalid_id, entity::invalid_version};

entity::entity(entity&& other) noexcept
: _id{std::exchange(other._id, invalid_id)},
  _version{std::exchange(other._version, invalid_version)} { }

entity& entity::operator=(entity&& other) noexcept {
  _id = std::exchange(other._id, invalid_id);
  _version = std::exchange(other._version, invalid_id);

  return *this;
}

entity::entity(const id_type id, const version_type version) noexcept
: _id{id},
  _version{version} { }

void entity::_increment_version() noexcept {
  SBX_ASSERT(*this != null, "Cannot increment version of null entity.");
  // [NOTE] KAJ 2022-03-03 14:39 - Check if it is viable to overflow the version.
  ++_version;
}

bool operator==(const entity& lhs, const entity& rhs) noexcept {
  return lhs._id == rhs._id && lhs._version == rhs._version;
}

} // namespace sbx
