#ifndef LIBSBX_UTILITY_EXIT_HPP_
#define LIBSBX_UTILITY_EXIT_HPP_

#include <cinttypes>

namespace sbx::utility {

/**
 * @brief Contains exit codes for the application.
 */
struct exit {
  inline static const auto success = std::int32_t{0};
  inline static const auto failure = std::int32_t{1};
}; // struct exit

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_EXIT_HPP_
