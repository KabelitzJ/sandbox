#ifndef SBX_ECS_ENTITY_SET_HPP_
#define SBX_ECS_ENTITY_SET_HPP_

#include <vector>
#include <memory>
#include <unordered_map>

#include <math/functional.hpp>

#include <platform/assert.hpp>

#include <meta/concepts.hpp>

#include "entity.hpp"

namespace sbx {

namespace detail {

template<container Container>
class entity_set_iterator final {

  using container_type = Container;

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

template<container Container>
[[nodiscard]] bool operator==(const entity_set_iterator<Container>& lhs, const entity_set_iterator<Container>& rhs) noexcept {
  return lhs.index() == rhs.index();
}

template<container Container>
[[nodiscard]] std::strong_ordering operator<=>(const entity_set_iterator<Container>& lhs, const entity_set_iterator<Container>& rhs) noexcept {
  return lhs.index() <=> rhs.index();
}

} // namespace detail

class entity_set {

  using sparse_container_type = std::unordered_map<entity::value_type, std::size_t>;
  using dense_container_type = std::vector<entity>;

public:

  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using reference = dense_container_type::const_reference;
  using pointer = dense_container_type::const_pointer;
  using iterator = detail::entity_set_iterator<dense_container_type>;
  using const_iterator = iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = reverse_iterator;

  entity_set() = default;

  entity_set(const entity_set& other) = delete;

  entity_set(entity_set&& other) noexcept = default;

  virtual ~entity_set() = default;

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

  void erase(const entity& entity) {
    _swap_and_pop(entity);
  }

  [[nodiscard]] bool contains(const entity& entity) const noexcept {
    if (const auto entry = _sparse.find(entity._value); entry != _sparse.cend()) {
      const auto& index = entry->second;
      return index < _dense.size() && _dense[index] == entity;
    }

    return false;
  }

  [[nodiscard]] iterator find(const entity& entity) const noexcept {
    return contains(entity) ? --(end() - static_cast<difference_type>(_index(entity))) : end(); 
  }

protected:

  virtual iterator _try_emplace(const entity& e) {
    _sparse.emplace(e._value, _dense.size());
    _dense.push_back(e);

    return begin();
  }

  virtual void _swap_and_pop(const entity& entity) {
    const auto idx = _index(entity);

    _sparse.at(_dense.back()._value) = idx;
    const auto old_entity = std::exchange(_dense[idx], _dense.back());

    _sparse.erase(old_entity._value);
    _dense.pop_back();
  }

  [[nodiscard]] size_type _index(const entity& entity) const noexcept {
    SBX_ASSERT(contains(entity), "Set does not contain entity");
    return _sparse.at(entity._value);
  }

private:

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class entity_set

} // namespace sbx

#endif // SBX_ECS_ENTITY_SET_HPP_
