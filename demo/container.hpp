#ifndef DEMO_CONTAINER_HPP_
#define DEMO_CONTAINER_HPP_

#include "type_less_container_base.hpp"

namespace demo {

namespace detail {

template<typename Container>
class container_iterator final {

  template<typename Type>
  friend bool operator==(const container_iterator<Type>&, const container_iterator<Type>&) noexcept;

  template<typename Type>
  friend std::strong_ordering operator<=>(const container_iterator<Type>&, const container_iterator<Type>&) noexcept;

  using container_type = Container;

public:

  using value_type = container_type::value_type;
  using size_type = container_type::size_type;
  using difference_type = container_type::difference_type;
  using pointer = value_type*;
  using reference = value_type&;

  container_iterator(container_type& container, const size_type index) noexcept
  : _container{container},
    _index{index} { }

  ~container_iterator() = default;

  container_iterator& operator++() noexcept {
    ++_index;
    return *this;
  }

  container_iterator operator++(int) noexcept {
    const auto copy = container_iterator{*this};
    ++_index;
    return copy;
  }

  container_iterator& operator--() noexcept {
    --_index;
    return *this;
  }

  container_iterator operator--(int) noexcept {
    const auto copy = container_iterator{*this};
    --_index;
    return copy;
  }

  pointer operator->() const noexcept {
    return &_container.at(_index);
  }

  reference operator*() const noexcept {
    return _container.at(_index);
  }

private:

  container_type& _container{};
  size_type _index{};

}; // class container_iterator

template<typename Container>
bool operator==(const container_iterator<Container>& lhs, const container_iterator<Container>& rhs) noexcept {
  return lhs._index == rhs._index;
}

template<typename Container>
std::strong_ordering operator<=>(const container_iterator<Container>& lhs, const container_iterator<Container>& rhs) noexcept {
  return lhs._index <=> rhs._index;
}

} // namespace detail

template<typename Type>
class container final : public type_less_container_base {

  using base_type = type_less_container_base;

public:

  using value_type = Type;
  using size_type = base_type::size_type;
  using difference_type = base_type::difference_type;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = detail::container_iterator<container<value_type>>;

  container() noexcept
  : base_type{sizeof(value_type)} { }

  container(const container& other) = delete;

  container(container&& other) noexcept
  : base_type{std::move(other)} { }

  ~container() {
    std::cout << "~container()" << std::endl;
    for (auto& element : *this) {
      std::destroy_at(std::addressof(element));
    }
  }

  container& operator=(const container& other) = delete;

  container& operator=(container&& other) noexcept {
    base_type::operator=(std::move(other));

    return *this;
  }

  template<typename... Args>
  requires (std::is_constructible_v<value_type, Args...>)
  reference emplace_back(Args&&... args) {
    const auto index = base_type::size();
    base_type::_push_back();

    auto* data = reinterpret_cast<value_type*>(base_type::_at(index));
    std::construct_at(data, std::forward<Args>(args)...);

    return *data;
  }

  void push_back(const value_type& value) {
    const auto index = base_type::size();
    base_type::_push_back();

    auto* data = reinterpret_cast<value_type*>(base_type::_at(index));
    
    std::construct_at(data, value);
  }

  void push_back(value_type&& value) {
    const auto index = base_type::size();
    base_type::_push_back();

    auto* data = reinterpret_cast<value_type*>(base_type::_at(index));
    
    std::construct_at(data, std::move(value));
  }

  void pop_back() {
    const auto index = base_type::size() - size_type{1};

    auto* data = reinterpret_cast<value_type*>(base_type::_at(index));

    std::destroy_at(data);

    base_type::_pop_back();
  }

  void remove(const size_type index) {
    auto* data = reinterpret_cast<value_type*>(base_type::_at(index));

    std::destroy_at(data);

    base_type::_remove(index);
  }

  reference at(const size_type index) {
    return *reinterpret_cast<value_type*>(base_type::_at(index));
  }

  const_reference at(const size_type index) const {
    return *reinterpret_cast<value_type*>(base_type::_at(index));
  }

  iterator begin() noexcept {
    return iterator{*this, 0};
  }

  iterator end() noexcept {
    return iterator{*this, size()};
  }

}; // class container

} // namespace demo

#endif // DEMO_CONTAINER_HPP_
