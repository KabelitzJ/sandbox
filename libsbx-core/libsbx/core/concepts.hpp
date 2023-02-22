#ifndef LIBSBX_CORE_CONCEPTS_HPP_
#define LIBSBX_CORE_CONCEPTS_HPP_

#include <type_traits>

namespace sbx::core {

/**
 * @brief  Describes a type or object that can be invoked with the give parameters and return the given type
 * 
 * @tparam Callable Type of the callable
 * @tparam Return Return type of the callable
 * @tparam Args... Types of the arguments of the callable
 */
template<typename Callable, typename Return, typename... Args>
concept callable = std::is_invocable_r_v<Return, Callable, Args...>;

} // namespace sbx::core

#endif // LIBSBX_CORE_CONCEPTS_HPP_
