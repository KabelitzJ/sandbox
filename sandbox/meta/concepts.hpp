#ifndef SBX_META_CONCEPTS_HPP_
#define SBX_META_CONCEPTS_HPP_

#include <concepts>
#include <memory>
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

template<typename Allocator, typename Type>
concept allocator = requires(Allocator& allocator, Type* value) {
  std::same_as<typename std::allocator_traits<Allocator>::value_type, Type>;
  std::same_as<typename std::allocator_traits<Allocator>::pointer, Type*>;
  { std::allocator_traits<Allocator>::allocate(allocator, std::size_t{1}) } -> std::same_as<Type*>;
  { std::allocator_traits<Allocator>::deallocate(allocator, value, std::size_t{1}) } -> std::same_as<void>;
};

} // namespace sbx

#endif // SBX_META_CONCEPTS_HPP_
