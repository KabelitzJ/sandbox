#ifndef LIBSBX_MEMORY_RING_BUFFER_HPP_
#define LIBSBX_MEMORY_RING_BUFFER_HPP_

#include <memory>
#include <utility>

namespace sbx::memory {

template<typename Type, std::size_t Capacity>
class ring_buffer {

public:

  using value_type = Type;

  auto capacity() const noexcept -> std::size_t {
    return Capacity;
  }

private:

  

}; // class ring_buffer

} // namespace sbx::memory

#endif // LIBSBX_MEMORY_RING_BUFFER_HPP_
