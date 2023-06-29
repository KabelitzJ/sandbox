#ifndef LIBSBX_STORAGE_HPP_
#define LIBSBX_STORAGE_HPP_

#include <type_traits>
#include <iostream>
#include <limits>
#include <vector>
#include <memory>

#include <libsbx/memory/concepts.hpp>
#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/ecs/sparse_set.hpp>

namespace sbx::ecs {

template<typename Key, typename Value, memory::allocator_for<Value> Allocator = std::allocator<Value>>
class storage : public sparse_set<Key, typename std::allocator_traits<Allocator>::rebind_alloc<Key>> {

  using allocator_traits = std::allocator_traits<Allocator>;


  using container_type = std::vector<Value, Allocator>;

public:

  using base_type = sparse_set<Key, memory::rebound_allocator_t<Allocator, Key>>;
  using key_type = Key;
  using value_type = Value;
  using reference = value_type&;
  using const_reference = const value_type&;

  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  storage()
  : base_type{} { }

  storage(const storage& other) = delete;

  storage(storage&& other) noexcept
  : base_type{std::move(other)},
    _values{std::move(other._values)} { }

  ~storage() {
    base_type::clear();
  }

  auto operator=(const storage& other) -> storage& = delete;

  auto operator=(storage&& other) noexcept -> storage& {
    if (this != &other) {
      base_type::operator=(std::move(other));
      _values = std::move(other._values);
    }

    return *this;
  }

  template<typename... Args>
  requires(std::constructible_from<Value, Args...>)
  auto add(const key_type& key, Args&&... args) -> memory::observer_ptr<value_type> {
    if (auto entry = find(key); entry != end()) {
      return memory::make_observer<value_type>(*entry = value_type{std::forward<Args>(args)...});
    }

    base_type::_emplace(key);

    if constexpr (std::is_aggregate_v<value_type>) {
      _values.emplace_back(value_type{std::forward<Args>(args)...});
    } else {
      _values.emplace_back(std::forward<Args>(args)...);
    }

    return memory::make_observer<value_type>(_values.back());
  }

  auto begin() -> iterator {
    return _values.begin();
  }

  auto begin() const -> const_iterator {
    return _values.begin();
  }

  auto cbegin() const -> const_iterator {
    return _values.cbegin();
  }

  auto end() -> iterator {
    return _values.end();
  }

  auto end() const -> const_iterator {
    return _values.end();
  }

  auto cend() const -> const_iterator {
    return _values.cend();
  }

  auto find(const key_type& key) -> iterator {
    if (base_type::contains(key)) {
      auto entry = begin();
      std::advance(entry, base_type::_index(key));
      return entry;
    }

    return end();
  }

  auto find(const key_type& key) const -> const_iterator {
    if (base_type::contains(key)) {
      auto entry = cbegin();
      std::advance(entry, base_type::_index(key));
      return entry;
    }

    return cend();
  }

  auto get(const key_type& key) -> reference {
    return *find(key);
  }

  auto get(const key_type& key) const -> const_reference {
    return *find(key);
  }

  auto as_tuple(const key_type& key) -> std::tuple<reference> {
    return std::forward_as_tuple(*find(key));
  }

  auto as_tuple(const key_type& key) const -> std::tuple<const_reference> {
    return std::forward_as_tuple(*find(key));
  }

protected:

  auto _swap_and_pop(const key_type& key) -> void override {
    const auto index = base_type::_index(key);

    using std::swap;
    swap(_values.at(index), _values.back());

    _values.pop_back();

    base_type::_swap_and_pop(key);
  }

  auto _clear() -> void override {
    base_type::_clear();
    _values.clear();
  }

private:

  container_type _values;

}; // class storage

} // namespace sbx::ecs

#endif // LIBSBX_STORAGE_HPP_
