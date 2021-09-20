#include "entity.hpp"

namespace sbx {

entity::entity(uint32 id)
: _id(id) {
  
}

entity::entity(uint16 index, uint16 version)
: _id(static_cast<uint32>(index << 16) | static_cast<uint32>(version)) {

}

uint32 entity::id() const {
  return _id;
}

uint16 entity::index() const {
  return static_cast<uint16>(_id >> 16);
}

uint16 entity::version() const {
  return static_cast<uint16>(_id);
}

entity::operator uint32() const {
  return _id;
}

} // namespace sbx
