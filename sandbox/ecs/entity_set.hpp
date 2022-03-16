#ifndef SBX_ECS_ENTITY_SET_HPP_
#define SBX_ECS_ENTITY_SET_HPP_

#include <vector>
#include <memory>
#include <optional>

#include <types/primitives.hpp>

#include "entity.hpp"

namespace sbx {

namespace detail {

struct entity_set_iterator final {

  using container_type = std::vector<entity>;

public:

  using value_type = entity;
  using size_type = container_type::size_type;
  using difference_type = container_type::difference_type;
  using const_pointer = container_type::const_pointer;
  using const_reference = container_type::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;

  entity_set_iterator(const container_type& data, const size_type offset) noexcept;

  entity_set_iterator(const entity_set_iterator& other) noexcept = default;

  entity_set_iterator(entity_set_iterator&& other) noexcept = default;

  ~entity_set_iterator() noexcept = default;

  entity_set_iterator& operator=(const entity_set_iterator& other) noexcept = default;

  entity_set_iterator& operator=(entity_set_iterator&& other) noexcept = default;

  entity_set_iterator& operator++() noexcept;

  [[nodiscard]] entity_set_iterator operator++(int) noexcept;

  entity_set_iterator& operator--() noexcept;

  [[nodiscard]] entity_set_iterator operator--(int) noexcept;

  entity_set_iterator& operator+=(const difference_type offset) noexcept;

  [[nodiscard]] entity_set_iterator operator+(const difference_type offset) const noexcept;

  entity_set_iterator& operator-=(const difference_type offset) noexcept;

  [[nodiscard]] entity_set_iterator operator-(const difference_type offset) const noexcept;

  [[nodiscard]] const_reference operator*() const noexcept;

  [[nodiscard]] const_pointer operator->() const noexcept;

  [[nodiscard]] size_type index() const noexcept;

private:

  const container_type* _data{};
  size_type _offset{};

}; // struct entity_set_iterator

bool operator==(const entity_set_iterator& lhs, const entity_set_iterator& rhs) noexcept;

} // namespace detail

class entity_set {

  using sparse_container_type = std::vector<std::size_t>;
  using dense_container_type = std::vector<entity>;

  inline static constexpr auto sparse_page_size = std::size_t{2048};

public:

  using size_type = dense_container_type::size_type;
  using difference_type = dense_container_type::difference_type;
  using const_reference = dense_container_type::const_reference;
  using const_pointer = dense_container_type::const_pointer;
  using const_iterator = detail::entity_set_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  entity_set() = default;

  entity_set(const entity_set& other) = delete;

  entity_set(entity_set&& other) noexcept = default;

  virtual ~entity_set() = default;

  entity_set& operator=(const entity_set& other) = delete;

  entity_set& operator=(entity_set&& other) noexcept = default;

  [[nodiscard]] size_type size() const noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept;

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept;

  [[nodiscard]] const_reverse_iterator rend() const noexcept;

  [[nodiscard]] const_reverse_iterator crend() const noexcept;

  const_iterator emplace(const entity& entity);

  void erase(const entity& entity);

  [[nodiscard]] const_iterator find(const entity& entity) const noexcept;

  [[nodiscard]] bool contains(const entity& entity) const noexcept;

  [[nodiscard]] size_type index(const entity& entity) const;

protected:

  virtual const_iterator _try_emplace(const entity& entity);

  virtual void _swap_and_pop(const_iterator first, const_iterator last);

private:

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class entity_set

} // namespace sbx

#endif // SBX_ECS_ENTITY_SET_HPP_
