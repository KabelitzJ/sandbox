#ifndef SBX_META_CONCEPTS_HPP_
#define SBX_META_CONCEPTS_HPP_

#include <concepts>
#include <type_traits>

namespace sbx {

template<typename Type>
concept arithmetic = std::is_arithmetic_v<Type>;

template<typename OutputStream, typename Type>
concept output_stream = requires(OutputStream& os, const Type& value) {
  { os << value } -> std::same_as<OutputStream&>;
};

template<typename InputStream, typename Type>
concept input_stream = requires(InputStream& os, Type& value) {
  { os >> value } -> std::same_as<InputStream&>;
};


} // namespace sbx

#endif // SBX_META_CONCEPTS_HPP_
