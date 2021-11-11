#ifndef SBX_CORE_UUID_HPP_
#define SBX_CORE_UUID_HPP_

#include <types/primitives.hpp>

namespace sbx {

/**
 * @brief Represents a universally unique identifier (UUID).
 * 
 * @warning This is not an actual uuid but it is sufficient for our purposes.
 */
class uuid {

public:
  /**
   * @brief Construct a new uuid object
   */
  uuid();

  /**
   * @brief Destroy the uuid object
   */
  ~uuid() = default;

  /**
   * @brief Converts the uuid to a {@ref uint64}.
   * 
   * @return uint64 
   */
  operator uint64() const;

private:

  uint64 _value{}; // Randomly generated value

}; // class uuid

} // namespace sbx

#endif // SBX_CORE_UUID_HPP_
