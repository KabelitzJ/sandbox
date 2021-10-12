#ifndef SBX_UTIL_TYPE_INDEX_HPP_
#define SBX_UTIL_TYPE_INDEX_HPP_

#include <atomic>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

/**
 * @brief A integer sequence with an incrementing value
 *
 * Primary template is left undefined on purpose
 */
template<typename, typename = void>
struct sequence;


/**
 * @brief A integer sequence with an incrementing value
 *
 * @tparam Type Integer type of the sequence
 */
template<typename Type>
struct sequence<Type, std::enable_if_t<std::is_unsigned_v<Type>>> {

  [[nodiscard]] static Type next() noexcept {
    static auto value = Type{0};

    return value++;
  }

}; // struct sequence


/**
 * @brief A integer sequence with an atomically incrementing value
 *
 * @tparam Type Integer type of the sequence
 */
template<typename Type>
using atomic_sequence = sequence<std::atomic<Type>>;


/**
 * @brief Maps any given type to an unsigned 32 bit identifier
 *
 * @warning Indices are not required to be the same for any given type throughout multiple program instances
 *
 * @tparam Type to get the index of
 */
template<typename Type>
struct type_index final {

  [[nodiscard]] static uint32 value() noexcept {
    static const auto value = sequence<uint32>::next();

    return value;
  }

  [[nodiscard]] constexpr operator uint32() const noexcept {
    return value();
  }

}; // struct type_index

} // namespace sbx

#endif // SBX_UTIL_TYPE_INDEX_HPP_
