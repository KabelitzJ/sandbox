#ifndef SBX_CONTAINER_DYNAMIC_BITSET_HPP_
#define SBX_CONTAINER_DYNAMIC_BITSET_HPP_

#include <type_traits>
#include <concepts>
#include <memory>
#include <vector>
#include <numeric>

#include <meta/concepts.hpp>

#include <types/primitives.hpp>

namespace sbx {

class dynamic_bitset {

  friend bool operator==(const dynamic_bitset& lhs, const dynamic_bitset& rhs) noexcept;

  using underlying_type = uint32;

  inline static constexpr auto underlying_type_bit_count = std::numeric_limits<underlying_type>::digits;

  using allocator_type = std::allocator<underlying_type>;
  using allocator_traits = std::allocator_traits<allocator_type>;

  using buffer_type = underlying_type*;

public:

  using size_type = std::size_t;

  dynamic_bitset() noexcept;

  dynamic_bitset(const size_type size);

  dynamic_bitset(const dynamic_bitset& other);

  dynamic_bitset(const std::initializer_list<size_type> values);

  dynamic_bitset(dynamic_bitset&& other) noexcept;

  ~dynamic_bitset();

  dynamic_bitset& operator=(const dynamic_bitset& other);

  dynamic_bitset& operator=(dynamic_bitset&& other) noexcept;

  void set(const size_type index);

  void clear(const size_type index);

  void reset();

  [[nodiscard]] bool test(const size_type index) const noexcept;

  [[nodiscard]] bool test(const dynamic_bitset& other) const noexcept;

private:

  void _resize(const size_type size);

  allocator_type _allocator{};
  buffer_type _buffer{};
  size_type _size{};

};

[[nodiscard]] bool operator==(const dynamic_bitset& lhs, const dynamic_bitset& rhs) noexcept;

} // namespace sbx

#endif // SBX_CONTAINER_DYNAMIC_BITSET_HPP_
