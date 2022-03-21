#ifndef SBX_ECS_ENTITY_SET_HPP_
#define SBX_ECS_ENTITY_SET_HPP_

#include <vector>
#include <memory>

#include <math/functional.hpp>

#include <platform/assert.hpp>

#include "entity.hpp"

namespace sbx {

namespace detail {

class entity_set_iterator final {

  using container_type = std::vector<entity>;

public:

  using value_type = container_type::value_type;
  using difference_type = container_type::difference_type;
  using pointer = container_type::const_pointer;
  using reference = container_type::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;

  entity_set_iterator(const container_type& container, const difference_type offset) noexcept
  : _container{std::addressof(container)}, 
    _offset{offset} { }

  ~entity_set_iterator() noexcept = default;

  entity_set_iterator operator++() noexcept {
    --_offset;
    return *this;
  }

  [[nodiscard]] entity_set_iterator operator++(int) noexcept {
    const auto copy = entity_set_iterator{*this};
    --_offset;
    return copy;
  }

  entity_set_iterator operator--() noexcept {
    ++_offset;
    return *this;
  }

  [[nodiscard]] entity_set_iterator operator--(int) noexcept {
    const auto copy = entity_set_iterator{*this};
    ++_offset;
    return copy;
  }

  entity_set_iterator& operator+=(const difference_type offset) noexcept {
    _offset -= offset;
    return *this;
  }

  [[nodiscard]] entity_set_iterator operator+(const difference_type offset) const noexcept {
    auto copy = entity_set_iterator{*this};
    copy += offset;
    return copy;
  }

  entity_set_iterator& operator-=(const difference_type offset) noexcept {
    _offset += offset;
    return *this;
  }

  [[nodiscard]] entity_set_iterator operator-(const difference_type offset) const noexcept {
    auto copy = entity_set_iterator{*this};
    copy -= offset;
    return copy;
  }

  [[nodiscard]] pointer operator->() const noexcept {
    return _container->data() + index();
  }

  [[nodiscard]] reference operator*() const noexcept {
    return _container->data()[index()];
  }

  [[nodiscard]] difference_type index() const noexcept {
    return _offset - difference_type{1};
  }

private:

  const container_type* _container{};
  difference_type _offset{};

}; // struct entity_set_iterator

[[nodiscard]] bool operator==(const entity_set_iterator& lhs, const entity_set_iterator& rhs) noexcept {
  return lhs.index() == rhs.index();
}

[[nodiscard]] std::strong_ordering operator<=>(const entity_set_iterator& lhs, const entity_set_iterator& rhs) noexcept {
  return lhs.index() <=> rhs.index();
}

} // namespace detail

class entity_set {

  using allocator_type = std::allocator<entity>;
  using allocator_traits = std::allocator_traits<allocator_type>;

  using sparse_container_type = std::vector<entity*>;
  using dense_container_type = std::vector<entity>;

  inline static constexpr auto sparse_page_size = std::size_t{2048};

public:

  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using reference = dense_container_type::const_reference;
  using pointer = dense_container_type::const_pointer;
  using iterator = detail::entity_set_iterator;
  using const_iterator = iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = reverse_iterator;

  entity_set() = default;

  entity_set(const entity_set& other) = delete;

  entity_set(entity_set&& other) noexcept = default;

  virtual ~entity_set() {
    _release_sparse_pages();
  }

  entity_set& operator=(const entity_set& other) = delete;

  entity_set& operator=(entity_set&& other) noexcept = default;

  [[nodiscard]] size_type size() const noexcept {
    return _dense.size();
  }

  [[nodiscard]] bool empty() const noexcept {
    return _dense.empty();
  }

  [[nodiscard]] const_iterator begin() const noexcept {
    return const_iterator{_dense, static_cast<difference_type>(_dense.size())};
  }

  [[nodiscard]] const_iterator cbegin() const noexcept {
    return begin();
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return const_iterator{_dense, difference_type{0}};
  }

  [[nodiscard]] const_iterator cend() const noexcept {
    return end();
  }

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return std::make_reverse_iterator(end());
  }

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }

  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return std::make_reverse_iterator(begin());
  }

  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return rend();
  }

  iterator insert(const entity& entity) {
    return _try_emplace(entity);
  }

  void erase(const entity& entity) {
    _swap_and_pop(entity);
  }

  [[nodiscard]] size_type index(const entity& entity) const noexcept {
    SBX_ASSERT(contains(entity), "set does not contain entity");
    return static_cast<size_type>(_sparse_element(entity)._id());
  }

  [[nodiscard]] bool contains(const entity& entity) const noexcept {
    const auto* element = _sparse_pointer(entity);
    return element && element->_id() < _dense.size() && _dense[element->_id()] == entity; 
  }

  [[nodiscard]] iterator find(const entity& entity) const noexcept {
    return contains(entity) ? --(end() - static_cast<difference_type>(index(entity))) : end(); 
  }

protected:

  virtual iterator _try_emplace(const entity& e) {
    const auto id = static_cast<entity::id_type>(_dense.size());
    _assure_sparse_element(e) = entity{id, e._version()};
    _dense.push_back(e);
    return begin();
  }

  virtual void _swap_and_pop(const entity& e) {
    const auto idx = index(e);
    const auto id = static_cast<entity::id_type>(idx);
    _sparse_element(_dense.back()) = entity(id, _dense.back()._version());
    const auto temp = std::exchange(_dense[idx], _dense.back());
    _sparse_element(temp) = entity::null;
    _dense.pop_back();
  }

private:

  entity& _assure_sparse_element(const entity& entity) {
    const auto index = static_cast<std::size_t>(entity._id());
    const auto page = index / sparse_page_size;

    if (page >= _sparse.size()) {
      _sparse.resize(page + 1, nullptr);
    }

    if (!_sparse[page]) {
      _sparse[page] = allocator_traits::allocate(_allocator, sparse_page_size);
      std::uninitialized_fill_n(_sparse[page], sparse_page_size, entity::null);
    }

    return _sparse[page][fast_mod(index, sparse_page_size)];
  }

  entity& _sparse_element(const entity& entity) const noexcept {
    SBX_ASSERT(_sparse_pointer(entity), "set does not contain entity");
    const auto index = static_cast<std::size_t>(entity._id());
    const auto page = index / sparse_page_size;
    return _sparse[page][fast_mod(index, sparse_page_size)];
  }

  entity* _sparse_pointer(const entity& entity) const noexcept {
    const auto index = static_cast<std::size_t>(entity._id());
    const auto page = index / sparse_page_size;
    return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod(index, sparse_page_size)) : nullptr;
  }

  void _release_sparse_pages() {
    for (auto* page : _sparse) {
      if (page) {
        allocator_traits::deallocate(_allocator, page, sparse_page_size);
        page = nullptr;
      }
    }
  }

  allocator_type _allocator{};
  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class entity_set

} // namespace sbx

#endif // SBX_ECS_ENTITY_SET_HPP_
