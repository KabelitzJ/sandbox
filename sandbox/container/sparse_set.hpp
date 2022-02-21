#ifndef SBX_CONTAINER_SPARSE_SET_HPP_
#define SBX_CONTAINER_SPARSE_SET_HPP_

#include <bit>
#include <concepts>
#include <vector>
#include <memory>

#include <meta/concepts.hpp>

namespace sbx {

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
requires (std::has_single_bit(PageSize))
class basic_sparse_set {

public:

private:

}; // class basic_sparse_set

template<std::unsigned_integral Type>
using sparse_set = basic_sparse_set<Type, std::allocator<Type>, std::size_t{4096}>;

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_CONTAINER_SPARSE_SET_HPP_
