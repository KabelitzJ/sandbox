#ifndef LIBSBX_UTILTY_TYPE_ID_HPP_
#define LIBSBX_UTILTY_TYPE_ID_HPP_

#include <cstdint>
#include <type_traits>

namespace sbx::utility {

namespace detail {

struct id_generator final {
  [[nodiscard]] static auto next() noexcept -> std::uint32_t {
    static auto id = std::uint32_t{};
    return id++;
  }
}; // struct type_index

} // namespace detail

template<typename Type>
struct type_id {

  [[nodiscard]] static auto value() noexcept -> std::uint32_t {
    static const auto value = detail::id_generator::next();
    return value;
  }

  [[nodiscard]] constexpr operator std::uint32_t() const noexcept {
    return value();
  }

}; // struct type_id

} // namespace sbx::utility

#endif // LIBSBX_UTILTY_TYPE_ID_HPP_
