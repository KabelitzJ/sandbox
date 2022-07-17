#ifndef SBX_MEMORY_POOL_STORAGE_HPP_
#define SBX_MEMORY_POOL_STORAGE_HPP_

#include <vector>
#include <memory>
#include <cstring>

#include <platform/assert.hpp>

#include <meta/concepts.hpp>

namespace sbx {

template<standard_layout Type, std::size_t ChunkSize>
class pool_allocator {

  using chunk_type = std::unique_ptr<std::byte, void(*)(std::byte*)>;

  inline static constexpr auto chunk_size = ChunkSize;

  struct node {
    node* next;
  };

  static_assert(sizeof(Type) >= sizeof(node), "Type is too small");

public:

  using value_type = Type;
  using size_type = std::size_t;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using void_pointer = void*;
  using const_void_pointer = const void*;
  using is_always_equal = std::false_type;

  pool_allocator()
  : _chunks{},
    _root{nullptr} {
    _allocate_chunk();
  }

  pool_allocator(const pool_allocator& other) = delete;

  pool_allocator(pool_allocator&& other) noexcept
  : _chunks{std::move(other._chunks)},
    _root{std::exchange(other._root, nullptr)} { }

  ~pool_allocator() = default;

  pool_allocator& operator=(const pool_allocator& other) = delete;

  pool_allocator& operator=(pool_allocator&& other) noexcept {
    if (this != &other) {
      _chunks = std::move(other._chunks);
      _root = std::exchange(other._root, nullptr);
    }

    return *this;
  }

  pointer allocate([[maybe_unused]] const size_type count) {
    SBX_ASSERT(count == 1, "Pool allocator can not allocate arrays");

    if (!_root) {
      _allocate_chunk();
    }

    auto* front = _root;
    _root = _root->next;
    return static_cast<pointer>(static_cast<void_pointer>(front));
  }

  void deallocate(pointer ptr, [[maybe_unused]] const size_type count) {
    SBX_ASSERT(count == 1, "Pool allocator can not deallocate arrays");

    auto* front = static_cast<node*>(static_cast<void_pointer>(ptr));
    std::memset(front, 0, sizeof(value_type));
    front->next = _root;
    _root = front;
  }

private:

  void _allocate_chunk() {
    auto chunk_deleter = [](auto* chunk) {
      std::free(static_cast<void_pointer>(chunk));
    };

    auto* chunk_memory = static_cast<std::byte*>(std::malloc(sizeof(value_type) * chunk_size));
    std::memset(chunk_memory, 0, sizeof(value_type) * chunk_size);

    _chunks.push_back(chunk_type{chunk_memory, chunk_deleter});

    const auto& chunk = _chunks.back();

    auto** current = &_root;

    for (auto i = size_type{0}; i < chunk_size; ++i) {
      *current = static_cast<node*>(static_cast<void_pointer>(chunk.get() + i * sizeof(value_type)));
      current = &((*current)->next);
    }
  }

  std::vector<chunk_type> _chunks{};
  node* _root{};

}; // class pool_allocator

template<typename Type, std::size_t Size>
constexpr bool operator==(const pool_allocator<Type, Size>& lhs, const pool_allocator<Type, Size>& rhs) noexcept {
  return false;
}

} // namespace sbx

#endif // SBX_MEMORY_POOL_STORAGE_HPP_
