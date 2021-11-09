#include "uuid.hpp"

#include <utils/random.hpp>

namespace sbx {

uuid::uuid()
: _value{random::next<uint64>()} { }

uuid::operator uint64() const {
  return _value;
}

} // namespace sbx
