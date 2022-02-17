#ifndef SBX_UTILS_TYPE_ID_HPP_
#define SBX_UTILS_TYPE_ID_HPP_

#include <atomic>
#include <concepts>

#include <types/primitives.hpp>

namespace sbx {

template<std::unsigned_integral Type>
struct id_sequence final {

  using id_type = Type;

  [[nodiscard]] static id_type next() noexcept;

}; // struct index_sequence

template<typename Sequence>
concept id_sequence_type = requires(Sequence seq) {
  typename Sequence::id_type;
  { Sequence::next() } -> std::same_as<typename Sequence::id_type>;
};

template<typename Type, typename Id = uint32, id_sequence_type Sequence = id_sequence<Id>>
struct type_id final {

  using id_type = Id;

  [[nodiscard]] static id_type value() noexcept;

  [[nodiscard]] constexpr operator id_type() const noexcept;

}; // struct type_index

} // namespace sbx

#include "type_id.inl"

#endif // SBX_UTILS_TYPE_ID_HPP_
