#ifndef SBX_CONTAINER_SPARSE_SET_HPP_
#define SBX_CONTAINER_SPARSE_SET_HPP_

#include <concepts>
#include <memory>
#include <vector>
#include <limits>

#include <math/functional.hpp>

#include <meta/concepts.hpp>

namespace sbx {

template<typename Allocator, typename Type>
concept allocator = requires (Allocator& allocator, Type* value) {
  std::same_as<typename std::allocator_traits<Allocator>::value_type, Type>;
  std::same_as<typename std::allocator_traits<Allocator>::pointer, Type*>;
  { std::allocator_traits<Allocator>::allocate(allocator, std::size_t{1}) } -> std::same_as<Type*>;
  { std::allocator_traits<Allocator>::deallocate(allocator, value, std::size_t{1}) } -> std::same_as<void>;
};

template<std::unsigned_integral Type, allocator<Type> Allocator, std::size_t PageSize>
requires (std::has_single_bit(PageSize))
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::rebind_alloc<typename allocator_traits::pointer>>;
  using dense_container_type = std::vector<Type, Allocator>;

  using pointer = typename dense_container_type::pointer;
  using reference = typename dense_container_type::reference;

  inline static constexpr auto page_size = PageSize;
  inline static constexpr auto placeholder = std::numeric_limits<Type>::max();

public:

  using value_type = typename dense_container_type::value_type;
  using allocator_type = typename dense_container_type::allocator_type;
  using size_type = typename dense_container_type::size_type;
  using difference_type = typename dense_container_type::difference_type;
  using const_reference = typename dense_container_type::const_reference;
  using const_pointer = typename dense_container_type::const_pointer;
  using const_iterator = typename dense_container_type::const_iterator;
  using const_reverse_iterator = typename dense_container_type::const_reverse_iterator;

  basic_sparse_set();

  basic_sparse_set(const allocator_type& allocator);

  basic_sparse_set(const basic_sparse_set& other) = delete;

  basic_sparse_set(basic_sparse_set&& other) noexcept;

  virtual ~basic_sparse_set();

  basic_sparse_set& operator=(const basic_sparse_set& other) = delete;

  basic_sparse_set& operator=(basic_sparse_set& other) noexcept;

  const_iterator begin() const {
    return _dense.begin();
  }

  const_iterator cbegin() const {
    return _dense.begin();
  }

  const_iterator end() const {
    return _dense.end();
  }

  const_iterator cend() const {
    return _dense.end();
  }

  const_iterator insert(const value_type value) {
    return _try_insert(value);
  }

  void remove(const value_type value) {
    _swap_and_pop(value);
  }

  bool contains(const value_type value) const {
    const auto sparse_element = _check_sparse_value(value);
    
    return sparse_element && (((~placeholder & value) ^ *sparse_element) < placeholder);
  }

  size_type index(const value_type value) const {
    assert(contains(value));
    return static_cast<size_type>(_sparse_reference(value));
  }

  const_iterator find(const value_type value) const {
    return contains(value) ? _dense.begin() + index(value) : end();
  }

protected:

  virtual const_iterator _try_insert(const value_type value) {
    if (const auto& element = find(value); element != end()) {
      return element;
    }

    const auto position = _dense.size();

    _dense.push_back(value);
    auto& sparse_element = _assure_at_least(value);
    sparse_element = position;

    return std::prev(end());
  }

  virtual void _swap_and_pop(const value_type value) {
    if(!contains(value)) {
      return;
    }

    const auto position = index(value);

    _sparse_reference(_dense.back()) = position;
    const auto packed_element = std::exchange(_dense[position], _dense.back());
    _sparse_reference(packed_element) = placeholder;

    _dense.pop_back();
  }

private:

  std::optional<value_type> _check_sparse_value(const value_type value) const {
    const auto position = static_cast<size_type>(value);
    const auto page = position / page_size;

    return (page < _sparse.size() && _sparse[page]) ? std::optional{_sparse[page][fast_mod(position, page_size)]} : std::nullopt;
  }

  reference _sparse_reference(const value_type value) const {
    assert(_check_sparse_value(value));

    const auto position = static_cast<size_type>(value);
    const auto page = position / page_size;

    return _sparse[page][fast_mod(position, page_size)];
  }

  reference _assure_at_least(const value_type value);

  void _release_sparse_pages();

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class basic_sparse_set

template<std::unsigned_integral Type>
using sparse_set = basic_sparse_set<Type, std::allocator<Type>, std::size_t{4096}>;

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_CONTAINER_SPARSE_SET_HPP_
