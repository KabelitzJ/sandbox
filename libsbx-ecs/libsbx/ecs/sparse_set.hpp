#ifndef LIBSBX_SPARSE_SET_HPP_
#define LIBSBX_SPARSE_SET_HPP_

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include <libsbx/memory/concepts.hpp>

namespace sbx::ecs {

template<typename Type, memory::allocator_for<Type> Allocator = std::allocator<Type>>
class sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using dense_storage_type = std::vector<Type, Allocator>;
  using sparse_storage_type = std::unordered_map<Type, std::size_t, std::hash<Type>, std::equal_to<Type>, memory::rebound_allocator_t<Allocator, std::pair<const Type, std::size_t>>>;

public:

  using size_type = std::size_t;
  using allocator_type = Allocator;
  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = dense_storage_type::iterator;
  using const_iterator = dense_storage_type::const_iterator;

  sparse_set() = default;

  sparse_set(const sparse_set& other) = delete;

  sparse_set(sparse_set&& other) noexcept
  : _dense{std::move(other._dense)},
    _sparse{std::move(other._sparse)} { }

  virtual ~sparse_set() {
    clear();
  }

  auto operator=(const sparse_set& other) -> sparse_set& = delete;

  auto operator=(sparse_set&& other) noexcept -> sparse_set& {
    if (this != &other) {
      _dense = std::move(other._dense);
      _sparse = std::move(other._sparse);
    }

    return *this;
  }

  auto contains(const_reference value) const -> bool {
    if (const auto entry = _sparse.find(value); entry != _sparse.cend()) {
      const auto index = entry->second;

      return index < _dense.size() && _dense.at(index) == value;
    }

    return false;
  }

  auto size() const noexcept -> size_type {
    return _dense.size();
  }

  auto at(size_type index) const -> const_reference {
    return _dense.at(index);
  }

  auto remove(const_reference value) -> void {
    if (!contains(value)) {
      return;
    }

    _swap_and_pop(value);
  }

  auto clear() -> void {
    _clear();
  }

  auto begin() -> iterator {
    return _dense.begin();
  }

  auto begin() const -> const_iterator {
    return _dense.begin();
  }

  auto cbegin() const -> const_iterator {
    return _dense.cbegin();
  }

  auto end() -> iterator {
    return _dense.end();
  }

  auto end() const -> const_iterator {
    return _dense.end();
  }

  auto cend() const -> const_iterator {
    return _dense.cend();
  }

protected:

  virtual auto _swap_and_pop(const_reference value) -> void {
    const auto index = _sparse.at(value);

    _sparse.at(_dense.back()) = index;
    const auto& old_value = std::exchange(_dense.at(index), _dense.back());

    _dense.pop_back();
    _sparse.erase(old_value);
  }

  virtual auto _clear() -> void {
    _dense.clear();
    _sparse.clear();
  }

  auto _emplace(const_reference value) -> void {
    const auto index = _dense.size();

    _sparse.insert({value, index});
    _dense.push_back(value);
  }

  auto _index(const_reference value) const -> size_type {
    if (const auto entry = _sparse.find(value); entry != _sparse.cend()) {
      return entry->second;
    }

    throw std::out_of_range{"Set does not contain value"};
  }

private:

  dense_storage_type _dense;
  sparse_storage_type _sparse;

}; // class sparse_set

} // namespace sbx::ecs

#endif // LIBSBX_SPARSE_SET_HPP_
