#ifndef SBX_ECS_TYPE_INDEX_HPP_
#define SBX_ECS_TYPE_INDEX_HPP_

#include <atomic>
#include <type_traits>

#include <types/primitives.hpp>

namespace sbx {

// [NOTE] KAJ 2021-09-26 15:08: Figure out why sequence is incomplete when its a template
struct sequence {

  [[nodiscard]] static uint32 next() noexcept {
    static auto value = uint32{};

    return value++;
  }

}; // struct sequence


template<typename Type>
struct type_index final {

  [[nodiscard]] static uint32 value() noexcept {
    static const auto value = sequence::next();

    return value;
  }

  [[nodiscard]] constexpr operator uint32() const noexcept {
    return value();
  }

}; // struct type_index

} // namespace sbx

#endif // SBX_ECS_TYPE_INDEX_HPP_
