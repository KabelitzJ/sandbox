#ifndef SBX_UTILS_TYPE_NAME_HPP_
#define SBX_UTILS_TYPE_NAME_HPP_

#include <string_view>

namespace sbx {

template<typename Type>
struct type_name {

  [[nodiscard]] constexpr static std::string_view value() noexcept;

}; // class type_name

} // namespace sbx

#include "type_name.inl"

#endif // SBX_UTILS_TYPE_NAME_HPP_
