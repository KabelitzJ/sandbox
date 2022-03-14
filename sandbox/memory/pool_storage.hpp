#ifndef SBX_MEMORY_POOL_STORAGE_HPP_
#define SBX_MEMORY_POOL_STORAGE_HPP_

#include <vector>
#include <memory>

#include "aligned_storage.hpp"

namespace sbx {

template<standard_layout Type, std::size_t PoolSize>
class pool_storage {

  inline static constexpr auto pool_size = PoolSize;

  using pool_type = aligned_storage<Type>[];

public:

  using value_type = Type;
  using size_type = std::size_t;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  pointer allocate() {
    if (_free_list.empty()) {
      _allocaate_pool();
    }

    auto data = _free_list.back();
    _free_list.pop_back();

    return data;
  }

  void deallocate(pointer data) {
    _free_list.push_back(data);
  }

private:

  void _allocaate_pool() {
    _pools.emplace_back(std::make_unique<pool_type>(pool_size));
    _free_list.reserve(_pools.size() * pool_size);

    auto& pool = _pools.back();

    for (auto i = 0u; i < pool_size; ++i) {
      _free_list.push_back(pool[i].data());
    }
  }

  std::vector<std::unique_ptr<pool_type>> _pools{};
  std::vector<pointer> _free_list{};

}; // class pool_storage

} // namespace sbx

#endif // SBX_MEMORY_POOL_STORAGE_HPP_
