#include <platform/target.hpp>

namespace sbx {

std::string_view _find_type_name(std::string_view prefix, std::string_view suffix, std::string_view function) {
  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find(suffix);
  const auto size = end - start;

  return function.substr(start, size);
}

template<typename Type>
constexpr std::string_view type_name<Type>::value() noexcept {
#if defined(SBX_COMPILER_CLANG)
  constexpr auto prefix = std::string_view{"[Type = "};
  constexpr auto suffix = std::string_view{"]"};
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
  
  return _find_type_name(prefix, suffix, function);
#elif defined(SBX_COMPILER_GNUC)
  constexpr auto prefix = std::string_view{"with Type = "};
  constexpr auto suffix = std::string_view{"; "};
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};

  return _find_type_name(prefix, suffix, function);
#elif defined(SBX_COMPILER_MSC)
  constexpr auto prefix = std::string_view{"value<"};
  constexpr auto suffix = std::string_view{">(void)"};
  constexpr auto function = std::string_view{__FUNCSIG__};

  return _find_type_name(prefix, suffix, function);
#else
  return std::string_view{typeid(Type).name()};
#endif
}

} // namespace sbx
