#ifndef SBX_CORE_UUID_HPP_
#define SBX_CORE_UUID_HPP_

#include <types/primitives.hpp>

namespace sbx {

class uuid {

public:
  uuid();
  ~uuid() = default;

  operator uint64() const;

private:

  uint64 _value{};

}; // class uuid

} // namespace sbx

#endif // SBX_CORE_UUID_HPP_
