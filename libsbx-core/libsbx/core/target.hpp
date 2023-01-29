#ifndef LIBSBX_CORE_TARGET_HPP_
#define LIBSBX_CORE_TARGET_HPP_

#include <cstdint>
#include <type_traits>

namespace sbx::core {

#if defined(__cpp_exceptions)
inline static constexpr auto has_exceptions_v = true;
#else
inline static constexpr auto has_exceptions_v = false;
#endif

/** @brief Possible build configurations */
enum class build_configuration : std::uint8_t {
  debug = 0,
  release = 1
}; // enum class build_configuration

#if defined(DEBUG) || !defined(NDEBUG)
inline static constexpr auto build_configuration_v = build_configuration::debug;
#else
inline static constexpr auto build_configuration_v = build_configuration::release;
#endif

/** @brief Possible operating systems */
enum class operating_system : std::uint8_t {
  windows = 0,
  max = 1,
  linux = 2,
  unknown = 3
}; // enum class operating_system

#if defined(WIN32) || defined(_WIN32)
inline static constexpr auto operating_system_v = operating_system::windows;
#elif defined(__APPLE__)
inline static constexpr auto operating_system_v = operating_system::mac;
#elif defined(__linux__) || defined(__linux)
inline static constexpr auto operating_system_v = operating_system::linux;
#else 
inline static constexpr auto operating_system_v = operating_system::unknown;
#endif

/** @brief Possible compilers */
enum class compiler : std::uint8_t {
  clang = 0,
  gnuc = 1,
  msc = 2,
  unknown = 3
}; // enum class compiler

#if defined(__clang__)
inline static constexpr auto compiler_v = compiler::clang;
#elif defined(__GNUC__)
inline static constexpr auto compiler_v = compiler::gnuc;
#elif defined(__MSC_VER)
inline static constexpr auto compiler_v = compiler::msc;
#else
inline static constexpr auto compiler_v = compiler::unknown;
#endif

} // namespace sbx::core

#endif // LIBSBX_CORE_TARGET_HPP_
