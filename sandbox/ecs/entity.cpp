#include "entity.hpp"

#include <utility>

#include <platform/assert.hpp>

namespace sbx {

const entity entity::null{entity::id_mask, entity::version_mask};

entity::entity(entity&& other) noexcept
: _value{std::exchange(other._value, null)} { }

entity& entity::operator=(entity&& other) noexcept {
  _value = std::exchange(other._value, null);

  return *this;
}

entity::operator value_type() const noexcept {
  return _value;
}

entity::entity(const id_type id, const version_type version) noexcept
: _value{static_cast<value_type>((id << id_shift) | version)} { }

void entity::_increment_version() noexcept {
  SBX_ASSERT(*this != null, "Cannot increment version of null entity.");

  const auto version = static_cast<version_type>(_version() + version_type{1});
  _value = static_cast<value_type>(_id() | version);
}

entity::id_type entity::_id() const noexcept {
  return static_cast<id_type>(_value >> id_shift);
}

entity::version_type entity::_version() const noexcept {
  return static_cast<version_type>(_value & version_mask);
}

} // namespace sbx
