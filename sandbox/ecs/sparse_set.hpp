#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <memory>
#include <utility>
#include <vector>
#include <type_traits>
#include <cassert>
#include <iterator>

#include <types/primitives.hpp>

#include <util/memory.hpp>

#include "entity.hpp"

namespace sbx {
  
template<typename Container>
struct sparse_set_iterator final {
  using container_type = Container;
  using value_type = typename container_type::value_type;
  using pointer = typename container_type::const_pointer;
  using reference = typename container_type::const_reference;
  using difference_type = typename container_type::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  sparse_set_iterator() noexcept = default;

  sparse_set_iterator(const container_type& container, const difference_type index) noexcept
  : _container{container},
    _index{index} { }

  sparse_set_iterator& operator++() noexcept {
    return --_index, *this;
  }

  sparse_set_iterator operator++(int) noexcept {
    const auto original = *this;
    return ++(*this), original;
  }

  sparse_set_iterator& operator--() noexcept {
    return ++_index, *this;
  }

  sparse_set_iterator operator--(int) noexcept {
    const auto original = *this;
    return --(*this), original;
  }

  sparse_set_iterator& operator+=(const difference_type value) noexcept {
    _index -= value;
    return *this;
  }

  sparse_set_iterator operator+(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy += value);
  }

  sparse_set_iterator& operator-=(const difference_type value) noexcept {
    _index += value;
    return *this;
  }

  sparse_set_iterator operator-(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy -= value);
  }

  difference_type operator-(const sparse_set_iterator& other) const noexcept {
    return other._index - _index;
  }

  [[nodiscard]] reference operator[](const difference_type value) const {
    return _container[value];
  }

  [[nodiscard]] bool operator==(const sparse_set_iterator& other) const noexcept {
    return other._index == _index;
  }

  [[nodiscard]] bool operator!=(const sparse_set_iterator& other) const noexcept {
    return !(*this == other);
  }

  [[nodiscard]] bool operator<(const sparse_set_iterator& other) const noexcept {
    return _index > other._index;
  }

  [[nodiscard]] bool operator>(const sparse_set_iterator& other) const noexcept {
    return _index < other._index;
  }

  [[nodiscard]] bool operator<=(const sparse_set_iterator& other) const noexcept {
    return !(*this > other);
  }

  [[nodiscard]] bool operator>=(const sparse_set_iterator& other) const noexcept {
    return !(*this < other);
  }

  [[nodiscard]] pointer operator->() const {
    const auto position = _index - 1;
    return _container.data() + position;
  }

  [[nodiscard]] reference operator*() const {
    return *operator->();
  }

private:
  const container_type& _container{};
  difference_type _index{};
};


template<typename Entity, typename = std::allocator<Entity>>
class basic_sparse_set;


using sparse_set = basic_sparse_set<entity>;


template<typename Entity, typename Allocator>
class basic_sparse_set {

  static constexpr auto sparse_page_v = 4096;

  using allocator_traits = std::allocator_traits<Allocator>;
  using alloc = typename allocator_traits::template rebind_alloc<Entity>;
  using alloc_traits = typename std::allocator_traits<alloc>;

  using entity_traits = sbx::entity_traits<Entity>;

  using sparse_container_type = std::vector<typename alloc_traits::pointer, typename alloc_traits::template rebind_alloc<typename alloc_traits::pointer>>;
  using packed_container_type = std::vector<Entity, alloc>;

public:

  using allocator_type = Allocator;
  using entity_type = Entity;
  using version_type = typename entity_traits::version_type;
  using size_type = typename packed_container_type::size_type;
  using pointer = typename packed_container_type::const_pointer;
  using iterator = sparse_set_iterator<packed_container_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using difference_type = typename iterator::difference_type;

  explicit basic_sparse_set(const allocator_type& allocator = allocator_type{})
  : _sparse{allocator},
    _packed{allocator},
    _free_list{tombstone_entity} { }

  basic_sparse_set(const basic_sparse_set&) = delete;

  basic_sparse_set(basic_sparse_set&& other) noexcept
  : _sparse{std::move(other._sparse)},
    _packed{std::move(other._packed)},
    _free_list{std::exchange(other._free_list, tombstone_entity)} { }

  virtual ~basic_sparse_set() {
    _release_sparse_pages();
  }

  basic_sparse_set& operator=(const basic_sparse_set&) = delete;

  basic_sparse_set& operator=(basic_sparse_set&& other) noexcept {
    assert(alloc_traits::is_always_equal::value || _packed.get_allocator() == other._packed.get_allocator());

    _release_sparse_pages();

    _sparse = std::move(other._sparse);
    _packed = std::move(other._packed);
    _free_list = std::exchange(other._free_list, tombstone_entity);

    return *this;
  }

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept {
    return _packed.get_allocator();
  }

  [[nodiscard]] size_type next_slot() const noexcept {
    return _free_list == null_entity ? _packed.size() : static_cast<size_type>(entity_traits::to_entity(_free_list));
  }

  virtual void reserve(const size_type capacity) {
    _packed.reserve(capacity);
  }

  [[nodiscard]] virtual size_type capacity() const noexcept {
    return _packed.capacity();
  }

  virtual void shrink_to_fit() {
    _packed.shrink_to_fit();
  }

  [[nodiscard]] size_type extend() const noexcept {
    return _sparse.size() * sparse_page_v;
  }

  [[nodiscard]] size_type size() const noexcept {
    return _packed.size();
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return _packed.empty();
  }

  [[nodiscard]] pointer data() const noexcept {
    return _packed.data();
  }

  [[nodiscard]] iterator begin() const noexcept {
    const auto position = static_cast<difference_type>(_packed.size());
    return iterator{_packed, position};
  }

  [[nodiscard]] iterator end() const noexcept {
    return iterator{_packed, difference_type{}};
  }

  [[nodiscard]] reverse_iterator rbegin() const noexcept {
    return std::make_reverse_iterator(begin());
  }

  [[nodiscard]] reverse_iterator rend() const noexcept {
    return std::make_reverse_iterator(end());
  }

  [[nodiscard]] bool contains(const entity_type entity) const noexcept {
    if (auto element = _sparse_pointer(entity); element) {
      constexpr auto capacity = entity_traits::to_entity(null_entity);

      return (((~capacity & entity_traits::to_integral(entity)) ^ entity_traits::to_integral(*element)) < capacity);
    }

    return false;
  }

  [[nodiscard]] size_type index(const entity_type entity) const noexcept {
    assert(contains(entity));
    return static_cast<size_type>(entity_traits::to_entity(_sparse_reference(entity)));
  }

  [[nodiscard]] iterator find(const entity_type entity) const noexcept {
    return contains(entity) ? --(end() - index(entity)) : end();  
  }

  [[nodiscard]] version_type current_version(const entity_type entity) const {
    if (auto element = sparse_ptr(entity); element) {
      return entity_traits::to_version(*element);
    }

    return entity_traits::to_version(tombstone_entity);
  }

  [[nodiscard]] entity_type at(const size_type position) const noexcept {
    return position < _packed.size() ? _packed[position] : null_entity;
  }

  [[nodiscard]] entity_type operator[](const size_type position) const noexcept {
    assert(position < _packed.size());
    return _packed[position];
  }

  void emplace(const entity_type entity) {
    _try_emplace(entity);
    assert(contains(entity));
  }

  template<typename Iterator>
  void emplace(Iterator first, Iterator last) {
    for (; first != last && _free_list != null_entity; ++first) {
      emplace(*first);
    }

    reserve(_packed.size() + std::distance(first, last));

    for (; first != last; ++first) {
      emplace(*first);
    }
  }

  void erase(const entity_type entity) {
    assert(contains(entity));
    _swap_and_pop(entity);
    assert(!contains(entity));
  }

  template<typename Iterator>
  void erase(Iterator first, Iterator last) {
    for (; first != last; ++first) {
      erase(*first);
    }
  }

  void clear() {
    for (auto& entity : this) {
      erase(entity);
    }
  }

protected:

  virtual void _swap_and_pop(const entity_type entity) {
    auto& reference = _sparse_reference(entity);
    const auto position = static_cast<size_type>(entity_traits::to_entity(reference));
    assert(_packed[position] == entity);

    _packed[position] = _packed.back();
    auto& element = _sparse_reference(_packed.back());
    element = entity_traits::combine(entity_traits::to_integral(reference), entity_traits::to_integral(element));
    
    reference = null_entity;
    
    assert((_packed.back() = tombstone_entity, true));
    _packed.pop_back();
  }

  virtual void _try_emplace(const entity_type entity) {
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    const auto page = position / sparse_page_v;

    if (page >= _sparse.size()) {
      _sparse.resize(page + 1u, nullptr);
    }

    if (!_sparse[page]) {
      auto page_allocator = _packed.get_allocator();
      _sparse[page] = alloc_traits::allocate(page_allocator, sparse_page_v);
      std::uninitialized_fill(_sparse[page], _sparse[page] + sparse_page_v, null_entity);
    }

    auto& element = _sparse[page][fast_mod<sparse_page_v>(position)];

    assert(entity_traits::to_version(element) == entity_traits::to_version(tombstone_entity));

    if (_free_list == null_entity) {
      element = entity_traits::combine(static_cast<typename entity_traits::entity_type>(_packed.size()), entity_traits::to_integral(entity));
      _packed.push_back(entity);
    } else {
      element = entity_traits::combine(entity_traits::to_integral(_free_list), entity_traits::to_integral(entity));
      _free_list = std::exchange(_packed[static_cast<size_type>(entity_traits::to_entity(_free_list))], entity);
    }
  }

private:

  [[nodiscard]] auto* _sparse_pointer(const entity_type entity) const {
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    const auto page = position / sparse_page_v;
    return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod<sparse_page_v>(position)) : nullptr;
  }

  [[nodiscard]] auto& _sparse_reference(const entity_type entity) const {
    assert(_sparse_pointer(entity));
    const auto position = static_cast<size_type>(entity_traits::to_entity(entity));
    return _sparse[position / sparse_page_v][fast_mod<sparse_page_v>(position)];
  }

  void _release_sparse_pages() {
    auto page_allocator = _packed.get_allocator();

    for (auto&& page: _sparse) {
      if (page != nullptr) {
        std::destroy(page, page + sparse_page_v);
        alloc_traits::deallocate(page_allocator, page, sparse_page_v);
        page = nullptr;
      }
    }
  }

  sparse_container_type _sparse{};
  packed_container_type _packed{};
  entity_type _free_list{};

};
  
} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
