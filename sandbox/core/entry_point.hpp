#ifndef SBX_CORE_ENTRY_POINT_HPP_
#define SBX_CORE_ENTRY_POINT_HPP_

#include <vector>
#include <string_view>

namespace sbx {

/**
 * @brief Will be used as custom setup.
 * 
 * @param cli_args Command line arguments that are passed to the executable
 */
extern void setup(const std::vector<std::string_view>& cli_args);

} // namespace sbx

#endif // SBX_CORE_ENTRY_POINT_HPP_
