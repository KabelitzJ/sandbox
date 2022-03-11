#include "entity.hpp"

#include <utility>

#include <platform/assert.hpp>

namespace sbx {

const entity entity::null{id_mask, version_mask};

entity::entity(entity&& other) noexcept
: _value{std::exchange(other._value, null._value)} { }

entity& entity::operator=(entity&& other) noexcept {
  _value = std::exchange(other._value, null._value);

  return *this;
}

entity::entity() noexcept
: _value{null._value} { }

entity::entity(const id_type id, const version_type version) noexcept
: _value{(id << version_shift) | version } { }

entity::id_type entity::_id() const noexcept {
  return _value >> version_shift;
}

entity::version_type entity::_version() const noexcept {
  return _value & version_mask;
}

void entity::_increment_version() noexcept {
  SBX_ASSERT(*this != null, "Cannot increment version of null entity");
  _value = (_value & id_mask) | ((_value + 1) & version_mask);
}

bool operator==(const entity& lhs, const entity& rhs) noexcept {
  return lhs._value == rhs._value;
}

} // namespace sbx
