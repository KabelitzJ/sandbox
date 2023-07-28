#ifndef LIBSBX_UTILITY_TARGET_HPP_
#define LIBSBX_UTILITY_TARGET_HPP_

#include <cstdint>
#include <type_traits>

namespace sbx::utility {

/** @brief Possible build configurations */
enum class build_configuration : std::uint8_t {
  debug = 0,
  release = 1
}; // enum class build_configuration

#if defined(DEBUG) || !defined(NDEBUG)
inline constexpr auto build_configuration_v = build_configuration::debug;
#else
inline constexpr auto build_configuration_v = build_configuration::release;
#endif

/** @brief Possible operating systems */
enum class operating_system : std::uint8_t {
  windows = 0,
  max = 1,
  linux = 2,
  unknown = 3
}; // enum class operating_system

#if defined(WIN32) || defined(_WIN32)
inline constexpr auto operating_system_v = operating_system::windows;
#elif defined(__APPLE__)
inline constexpr auto operating_system_v = operating_system::mac;
#elif defined(__linux__) || defined(__linux)
inline constexpr auto operating_system_v = operating_system::linux;
#else 
inline constexpr auto operating_system_v = operating_system::unknown;
#endif

/** @brief Possible compilers */
enum class compiler : std::uint8_t {
  clang = 0,
  gnuc = 1,
  msc = 2,
  unknown = 3
}; // enum class compiler

#if defined(__clang__)
inline constexpr auto compiler_v = compiler::clang;
#elif defined(__GNUC__)
inline constexpr auto compiler_v = compiler::gnuc;
#elif defined(__MSC_VER)
inline constexpr auto compiler_v = compiler::msc;
#else
inline constexpr auto compiler_v = compiler::unknown;
#endif

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_TARGET_HPP_
