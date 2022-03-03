#include "entity.hpp"

#include <utility>

#include <platform/assert.hpp>

namespace sbx {

const entity entity::null{id_mask, version_mask};

entity::entity(entity&& other) noexcept
: _value{std::exchange(other._value, (static_cast<value_type>(id_mask << id_shift) | version_mask))} { }

entity& entity::operator=(entity&& other) noexcept {
  _value = std::exchange(other._value, (static_cast<value_type>(id_mask << id_shift) | version_mask));

  return *this;
}

entity::entity(const id_type id, const version_type version) noexcept
: _value{static_cast<value_type>((id << id_shift)) | version} { }

void entity::_increment_version() noexcept {
  SBX_ASSERT(*this != null, "Cannot increment version of null entity.");
  // [NOTE] KAJ 2022-03-03 14:39 - Check if it is viable to overflow the version.
  _value = (_value & static_cast<value_type>(~version_mask)) | (static_cast<version_type>(_value & version_mask) + version_type{1});
}

entity::id_type entity::_id() const noexcept {
  return static_cast<id_type>(_value >> id_shift);
}

entity::version_type entity::_version() const noexcept {
  return static_cast<version_type>(_value & version_mask);
}

bool operator==(const entity& lhs, const entity& rhs) noexcept {
  return lhs._value == rhs._value;
}

} // namespace sbx
