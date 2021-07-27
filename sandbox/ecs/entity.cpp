#include "entity.hpp"

namespace sbx {

entity::entity(std::uint16_t index, std::uint16_t version)
: _id(static_cast<std::uint32_t>(index << 16) | static_cast<std::uint32_t>(version)) {

}

std::uint32_t entity::id() const {
  return _id;
}

std::uint16_t entity::index() const {
  return static_cast<std::uint16_t>(_id >> 16);
}

std::uint16_t entity::version() const {
  return static_cast<std::uint16_t>(_id);
}

} // namespace sbx
