#ifndef SBX_MEMORY_TYPELESS_POOL_HPP_
#define SBX_MEMORY_TYPELESS_POOL_HPP_

#include <vector>
#include <memory>

namespace sbx {

class typeless_pool {

  using pool_type = std::unique_ptr<std::byte, void(*)(std::byte*)>;

public:

  typeless_pool(const std::size_t element_size, const std::size_t pool_size)
  : _element_size{element_size},
    _pool_size{pool_size},
    _pools{},
    _free_list{} { }

  void* allocate() {
    if (_free_list.empty()) {
      _allocate_pool();
    }

    auto* data = _free_list.back();
    _free_list.pop_back();

    return static_cast<void*>(data);
  }

  void deallocate(void* data) {
    _free_list.push_back(static_cast<std::byte*>(data));
  }

private:

  void _allocate_pool() {
    _pools.push_back(pool_type{new std::byte[_pool_size * _element_size], [](auto* data){ delete[] data; }});

    _free_list.reserve(_pools.size() * _pool_size);

    auto& pool = _pools.back();

    for (auto i = 0u; i < _pool_size; ++i) {
      _free_list.push_back(pool.get() + i * _element_size);
    }
  }

  std::size_t _element_size{};
  std::size_t _pool_size{};
  std::vector<pool_type> _pools{};
  std::vector<std::byte*> _free_list{};

};

} // namespace sbx

#endif // SBX_MEMORY_TYPELESS_POOL_HPP_
