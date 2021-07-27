#ifndef SBX_CORE_ENTRY_POINT_HPP_
#define SBX_CORE_ENTRY_POINT_HPP_

#include <vector>
#include <memory>

#include "engine.hpp"

namespace sbx {

/**
 * @brief Will be used as custom setup.
 * 
 * @param cli_args Command line arguments that are passed to the executable
 */
extern void setup(sbx::engine& engine);

} // namespace sbx

#endif // SBX_CORE_ENTRY_POINT_HPP_
