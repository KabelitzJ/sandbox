#ifndef SBX_META_TYPE_GUARDS_HPP_
#define SBX_META_TYPE_GUARDS_HPP_

#include <type_traits>

/**
 * @brief Checks a template parameter if its type is arithemtic
 *
 * @param Type Type of the template parameter to check
 */
#define IS_ARITHMETIC(Type) typename = std::enable_if_t<std::is_arithmetic_v<Type>>

/**
 * @brief Checks a template parameter if its type is integral
 *
 * @param Type Type of the template parameter to check
 */
#define IS_INTEGRAL(Type) typename = std::enable_if_t<std::is_integral_v<Type>>

/**
 * @brief Checks a template parameter if its type is floating-point
 *
 * @param Type Type of the template parameter to check
 */
#define IS_FLOATING_POINT(Type) typename = std::enable_if_t<std::is_floating_point_v<Type>>

/**
 * @brief Checks a template parameter if its type is the same as a given type
 *
 * @param Type Type of the template parameter to check
 * @param TargetType Type to check against
 */
#define IS_SAME(Type, TargetType) typename = std::enable_if_t<std::is_same_v<Type, TargetType>>

#endif // SBX_META_TYPE_GUARDS_HPP_
