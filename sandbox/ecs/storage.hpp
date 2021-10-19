#ifndef SBX_ECS_STORAGE_HPP_
#define SBX_ECS_STORAGE_HPP_

#include <iterator>
#include <type_traits>

#include <util/memory.hpp>

#include "component.hpp"
#include "sparse_set.hpp"

namespace sbx {

template<typename Container>
class storage_iterator final {

  static constexpr auto packed_page_v = 1024;

  using container_type = std::remove_const_t<Container>;
  using allocator_traits = std::allocator_traits<typename container_type::allocator_type>;

  using iterator_traits = std::iterator_traits<std::conditional_t<
    std::is_const_v<Container>,
    typename allocator_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::const_pointer,
    typename allocator_traits::template rebind_traits<typename std::pointer_traits<typename container_type::value_type>::element_type>::pointer>
  >;

public:
  using difference_type = typename iterator_traits::difference_type;
  using value_type = typename iterator_traits::value_type;
  using pointer = typename iterator_traits::pointer;
  using reference = typename iterator_traits::reference;
  using iterator_category = std::random_access_iterator_tag;

  storage_iterator() noexcept = default;

  storage_iterator(Container& packed, difference_type index) noexcept
  : _packed{packed},
    _index{index} {}

  storage_iterator& operator++() noexcept {
    return --_index, *this;
  }

  storage_iterator operator++(int) noexcept {
    const auto original = *this;
    return ++(*this), original;
  }

  storage_iterator& operator--() noexcept {
      return ++_index, *this;
  }

  storage_iterator operator--(int) noexcept {
    const auto original = *this;
    return --(*this), original;
  }

  storage_iterator& operator+=(const difference_type value) noexcept {
    _index -= value;
    return *this;
  }

  storage_iterator operator+(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy += value);
  }

  storage_iterator& operator-=(const difference_type value) noexcept {
    _index += value;
    return *this;
  }

  storage_iterator operator-(const difference_type value) const noexcept {
    const auto copy = *this;
    return (copy -= value);
  }

  difference_type operator-(const storage_iterator& other) const noexcept {
    return other._index - _index;
  }

  [[nodiscard]] reference operator[](const difference_type value) const noexcept {
      return *operator+(value);
  }

  [[nodiscard]] bool operator==(const storage_iterator& other) const noexcept {
      return other._index == _index;
  }

  [[nodiscard]] bool operator!=(const storage_iterator& other) const noexcept {
      return !(*this == other);
  }

  [[nodiscard]] bool operator<(const storage_iterator& other) const noexcept {
      return _index > other._index;
  }

  [[nodiscard]] bool operator>(const storage_iterator& other) const noexcept {
    return _index < other._index;
  }

  [[nodiscard]] bool operator<=(const storage_iterator& other) const noexcept {
    return !(*this > other);
  }

  [[nodiscard]] bool operator>=(const storage_iterator& other) const noexcept {
    return !(*this < other);
  }

  [[nodiscard]] pointer operator->() const noexcept {
    const auto position = _index - 1;
    return (*_packed)[position / packed_page_v] + fast_mod<packed_page_v>(position);
  }

  [[nodiscard]] reference operator*() const noexcept {
    return *operator->();
  }

private:
  Container& _packed{};
  difference_type _index{};
}; // class storage_iterator


template<typename, typename Type, typename = std::allocator<Type>, typename = void>
class basic_storage;


template<typename Entity, typename Type, typename Allocator, typename>
class basic_storage {

  static constexpr auto packed_page_v = 1024;

  using allocator_traits = std::allocator_traits<Allocator>;
  using alloc = typename allocator_traits::template rebind_alloc<Type>;
  using alloc_traits = typename std::allocator_traits<alloc>;

  using entity_traits = entity_traits<Entity>;
  using component_traits = component_traits<Type>;
  using underlying_type = basic_sparse_set<Entity, typename allocator_traits::template rebind_alloc<Entity>>;
  using container_type = std::vector<typename alloc_traits::pointer, typename alloc_traits::template rebind_alloc<typename alloc_traits::pointer>>;

public:

  using base_type = underlying_type;
  using allocator_type = Allocator;
  using value_type = Type;
  using entity_type = Entity;
  using size_type = std::size_t;
  using pointer = typename container_type::pointer;
  using const_pointer = typename alloc_traits::template rebind_traits<typename alloc_traits::const_pointer>::const_pointer;
  using iterator = storage_iterator<container_type>;
  using const_iterator = storage_iterator<const container_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  basic_storage()
  : base_type{},
    _packed{} { }

  explicit basic_storage(const allocator_type& allocator)
  : base_type{allocator},
    _packed{container_type{allocator}, allocator} { }

  basic_storage(const basic_storage&) = delete;

  basic_storage(basic_storage&& other)
  : base_type{std::move(other)},
    _packed{std::move(other._packed)} { }

  ~basic_storage() override {
    _release_all_pages();
  }

  basic_storage& operator=(const basic_storage&) = delete;

  basic_storage& operator=(basic_storage&& other) {
    assert(alloc_traits::is_always_equal::value || _packed.second == other._packed.second);

    _release_all_pages();

    base_type::operator=(std::move(other));
    _packed = std::move(other._packed);

    return *this;
  }

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept {
    return allocator_type{_packed.second};
  }

  void reserve(const size_type capacity) override {
    base_type::reserve(capacity);

    if (capacity > base_type::size()) {
      _assure_at_least(capacity - 1u);
    }
  }

  void shrink_to_fit() override {
    base_type::shrink_to_fit();
    _release_unused_pages();
  }

  [[nodiscard]] const_pointer data() const noexcept {
    return _packed.first.data();
  }

  [[nodiscard]] pointer data() noexcept {
    return _packed.first.data();
  }

  [[nodiscard]] const_iterator cbegin() const noexcept {
    const auto position = static_cast<typename iterator::difference_type>(base_type::size());
    return const_iterator{&_packed.first, position};
  }

  [[nodiscard]] const_iterator begin() const noexcept {
    return cbegin();
  }

  [[nodiscard]] iterator begin() noexcept {
    const auto position = static_cast<typename iterator::difference_type>(base_type::size());
    return iterator{&_packed.first, position};
  }

  [[nodiscard]] const_iterator cend() const noexcept {
    return const_iterator{&_packed.first, {}};
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return cend();
  }

  [[nodiscard]] iterator end() noexcept {
    return iterator{&_packed.first, {}};
  }

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return std::make_reverse_iterator(cend());
  }

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return crbegin();
  }

  [[nodiscard]] reverse_iterator rbegin() noexcept {
    return std::make_reverse_iterator(end());
  }

  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return std::make_reverse_iterator(cbegin());
  }

  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return crend();
  }

  [[nodiscard]] reverse_iterator rend() noexcept {
    return std::make_reverse_iterator(begin());
  }

  [[nodiscard]] const value_type& get(const entity_type entity) const noexcept {
    return _element_at(base_type::index(entity));
  }

  [[nodiscard]] value_type& get(const entity_type entity) noexcept {
    return const_cast<value_type&>(std::as_const(*this).get(entity));
  }

  [[nodiscard]] std::tuple<const value_type&> get_as_tuple(const entity_type entity) const noexcept {
    return std::forward_as_tuple(get(entity));
  }

  [[nodiscard]] std::tuple<value_type&> get_as_tuple(const entity_type entity) noexcept {
    return std::forward_as_tuple(get(entity));
  }

  template<typename... Args>
  value_type& emplace(const entity_type entity, Args&&... args) {
    const auto position = base_type::slot();
    auto element = assure_at_least(position);

    construct(element, std::forward<Args>(args)...);

    base_type::try_emplace(entity, nullptr);
    assert(position == base_type::index(entity));

    return *element;
  }

  template<typename... Functions>
  value_type& patch(const entity_type entity, Functions&&... functions) {
    const auto index = base_type::index(entity);
    auto& element = element_at(index);

    (std::forward<Functions>(functions)(element), ...);

    return element;
  }

  template<typename Iterator>
  void insert(Iterator first, Iterator last, const value_type& value = value_type{}) {
    _consume_range(std::move(first), std::move(last), [&value]() -> decltype(auto) { return value; });
  }

  template<typename EntityIterator, typename ComponentIterator, typename = std::enable_if_t<std::is_same_v<std::decay_t<typename std::iterator_traits<ComponentIterator>::value_type>, value_type>>>
  void insert(EntityIterator first, EntityIterator last, ComponentIterator from) {
    _consume_range(std::move(first), std::move(last), [&from]() -> decltype(auto) { return *(from++); });
  }

protected:

  void _swap_and_pop(const Entity entity) override {
    const auto position = base_type::index(entity);
    const auto last = base_type::size() - 1u;

    auto& target = element_at(position);
    auto& element = element_at(last);

    [[maybe_unused]] auto unused = std::move(target);
    target = std::move(element);
    std::destroy_at(std::addressof(element));

    base_type::_swap_and_pop(entity);
  }

  void _try_emplace([[maybe_unused]] const Entity entity) override {
    if constexpr (std::is_default_constructible_v<value_type>) {
      const auto position = base_type::slot();
      construct(_assure_at_least(position));

      base_type::_try_emplace(entity);
      assert(position == base_type::index(entity));
    }
  }

private:

  [[nodiscard]] auto& _element_at(const size_type position) const {
    return _packed.first[position / packed_page_v][fast_mod<packed_page_v>(position)];
  }

  auto _assure_at_least(const size_type position) {
    auto& container = _packed.first;
    auto& page_allocator = _packed.second;
    const auto index = position / packed_page_v;

    if (index >= container.size()) {
      auto current = container.size();
      container.resize(index + 1u, nullptr);

      for (const auto last = container.size(); current < last; ++current) {
        container[current] = alloc_traits::allocate(page_allocator, packed_page_v);
      }
      
      return container[index] + fast_mod<packed_page_v>(position);
    }
  }

  void _release_unused_pages() {
    auto& container = _packed.first;
    auto& page_allocator = _packed.second;
    const auto in_use = (base_type::size() + packed_page_v - 1u) / packed_page_v;

    for (auto position = in_use, last = container.size(); position < last; ++position) {
      alloc_traits::deallocate(page_allocator, container[position], packed_page_v);
    }

    container.resize(in_use);
  }

  void _release_all_pages() {
    for (auto position = 0u, last = base_type::size(); position < last; ++position) {
      if constexpr (component_traits::in_place_delete::value) {
        if (base_type::at(position) != tombstone_entity) {
          std::destroy_at(std::addressof(_element_at( position)));
        }
      } else {
        std::destroy_at(std::addressof(_element_at( position)));
      }
    }

    auto& container = _packed.first;
    auto& page_allocator = _packed.second;

    for (auto position = 0u, last = container.size(); position < last; ++position) {
      alloc_traits::deallocate(page_allocator, container[position], packed_page_v);
    }
  }

  template<typename... Args>
  void _construct(typename alloc_traits::pointer location, Args&&... args) {
    auto& page_allocator = _packed.second;

    if constexpr(std::is_aggregate_v<value_type>) {
      alloc_traits::construct(page_allocator, to_address(location), Type{std::forward<Args>(args)...});
    } else {
      alloc_traits::construct(page_allocator, to_address(location), std::forward<Args>(args)...);
    }
  }

  template<typename Iterator, typename Generator>
  void _consume_range(Iterator first, Iterator last, Generator generator) {
    for (const auto size = base_type::size(); first != last && base_type::slot() != size; ++first) {
      emplace(*first, generator());
    }

    const auto required = base_type::size() + std::distance(first, last);
    base_type::reserve(required);
    reserve(required);

    for (; first != last; ++first) {
      emplace(*first, generator());
    }
  }

  std::pair<container_type, alloc> _packed{};

}; // class basic_storage


template<typename Entity, typename Type, typename Allocator>
class basic_storage<Entity, Type, Allocator, std::enable_if_t<ignore_as_empty_v<Type>>> 
: public basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>> {

  using allocator_traits = std::allocator_traits<Allocator>;
  using component_traits = component_traits<Type>;

public:

  using base_type = basic_sparse_set<Entity, typename allocator_traits::template rebind_alloc<Entity>>;
  using allocator_type = Allocator;
  using value_type = Type;
  using entity_type = Entity;
  using size_type = std::size_t;

  basic_storage()
  : base_type{} { }

  basic_storage(const allocator_type& allocator)
  : base_type{allocator} { }

  basic_storage(const basic_storage&) = delete;

  basic_storage(basic_storage&&) = default;

  basic_storage& operator=(const basic_storage&) = delete;

  basic_storage& operator=(basic_storage&&) = default;

  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept {
      return allocator_type{base_type::get_allocator()};
  }

  void get([[maybe_unused]] const entity_type entity) const noexcept {
    assert(base_type::contains(entity));
  }

  [[nodiscard]] std::tuple<> get_as_tuple([[maybe_unused]] const entity_type entity) const noexcept {
    assert(base_type::contains(entity));
    return std::tuple{};
  }

  template<typename... Args>
  void emplace(const entity_type entity, Args&&... args) {
    [[maybe_unused]] const value_type element{std::forward<Args>(args)...};
    base_type::_try_emplace(entity);
  }

  template<typename... Functions>
  void patch([[maybe_unused]] const entity_type entity, Functions&&... functions) {
    assert(base_type::contains(entity));
    (std::forward<Functions>(functions)(), ...);
  }

  template<typename Iterator, typename... Args>
  void insert(Iterator first, Iterator last, Args&&...) {
    for (const auto size = base_type::size(); first != last && base_type::slot() != size; ++first) {
      emplace(*first);
    }

    base_type::reserve(base_type::size() + std::distance(first, last));

    for (; first != last; ++first) {
      emplace(*first);
    }
  }

}; // class basic_storage

template<typename Entity, typename Type, typename = void>
struct storage_traits {
  using storage_type = basic_storage<Entity, Type>;
};

} // namespace sbx

#endif // SBX_ECS_STORAGE_HPP_
