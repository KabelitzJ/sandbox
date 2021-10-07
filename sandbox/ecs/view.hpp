#ifndef SBX_ECS_VIEW_HPP_
#define SBX_ECS_VIEW_HPP_

#include <type_traits>
#include <array>
#include <iterator>
#include <cassert>

#include <types/type_traits.hpp>

#include "component.hpp"
#include "iterable_storage.hpp"
#include "storage.hpp"

namespace sbx {


template<typename Type, typename Iterator, std::size_t Component>
class view_iterator final {
  static constexpr auto is_multi_type_v = (Component != 0u);

public:
  using iterator_type = Iterator;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using iterator_category = std::bidirectional_iterator_tag;

  view_iterator() noexcept
  : _first{},
    _last{},
    _current{},
    _pools{} { }

  view_iterator(Iterator from, Iterator to, Iterator current, std::array<const Type *, Component> all_of) noexcept
  : _first{from},
    _last{to},
    _current{current},
    _pools{all_of} {
    if(_current != _last && !_is_valid()) {
      ++(*this);
    }
  }

    view_iterator& operator++() noexcept {
      while(++_current != _last && !_is_valid()) {}
      return *this;
    }

    view_iterator operator++(int) noexcept {
      auto original = *this;
      return ++(*this), original;
    }

    view_iterator& operator--() noexcept {
        while(--_current != _first && !_is_valid()) {}
        return *this;
    }

    view_iterator operator--(int) noexcept {
        view_iterator original = *this;
        return operator--(), original;
    }

    [[nodiscard]] bool operator==(const view_iterator &other) const noexcept {
        return other._current == _current;
    }

    [[nodiscard]] bool operator!=(const view_iterator &other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] pointer operator->() const {
        return &*_current;
    }

    [[nodiscard]] reference operator*() const {
        return *operator->();
    }

private:
  [[nodiscard]] bool _is_valid() const {
    return (is_multi_type_v || (*_current != tombstone)) 
      && std::apply([entity = *_current](const auto*... current) { return (current->contains(entity) && ...); }, _pools);
  }

  Iterator _first;
  Iterator _last;
  Iterator _current;
  std::array<const Type*, Component> _pools;
};


template<typename, typename, typename = void>
class basic_view;

template<typename Entity, typename... Components>
class basic_view<Entity, type_list<Components...>> {

  static constexpr auto is_multi_type_v = (sizeof...(Components) != 1);

  using basic_common_type = std::common_type_t<typename basic_storage<Entity, std::remove_const_t<Components>>::base_type...>;

  class iterable final {
    template<typename Iterator>
    struct iterable_iterator final {
      using difference_type = std::ptrdiff_t;
      using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<basic_view>().get({})));
      using pointer = void;
      using reference = value_type;
      using iterator_category = std::input_iterator_tag;

      iterable_iterator(Iterator from, const basic_view *parent) noexcept
      : _iterator{from},
        _view{parent} { }

      iterable_iterator& operator++() noexcept {
        return ++_iterator, *this;
      }

      iterable_iterator operator++(int) noexcept {
        auto original = *this;
        return ++(*this), original;
      }

      [[nodiscard]] reference operator*() const noexcept {
        return std::tuple_cat(std::make_tuple(*_iterator), _view->get(*_iterator));
      }

      [[nodiscard]] bool operator==(const iterable_iterator& other) const noexcept {
        return other._iterator == _iterator;
      }

      [[nodiscard]] bool operator!=(const iterable_iterator& other) const noexcept {
        return !(*this == other);
      }

    private:
      Iterator _iterator;
      const basic_view* _view;

    };

public:
    using iterator = iterable_iterator<view_iterator<basic_common_type, typename basic_common_type::iterator, sizeof...(Components) - 1u>>;
    using reverse_iterator = iterable_iterator<view_iterator<basic_common_type, typename basic_common_type::reverse_iterator, sizeof...(Components) - 1u>>;

    iterable(const basic_view& parent)
    : _view{parent} { }

    [[nodiscard]] iterator begin() const noexcept {
      return iterator{_view.begin(), &_view};
    }

    [[nodiscard]] iterator end() const noexcept {
      return iterator{_view.end(), &_view};
    }

    [[nodiscard]] reverse_iterator rbegin() const noexcept {
      return reverse_iterator{_view.rbegin(), &_view};
    }

    [[nodiscard]] reverse_iterator rend() const noexcept {
      return reverse_iterator{_view.rend(), &_view};
    }

  private:
    const basic_view _view;

  };

public:
  using entity_type = Entity;
  using size_type = std::size_t;
  using iterator = view_iterator<basic_common_type, typename basic_common_type::iterator, sizeof...(Components) - 1u>;
  using reverse_iterator = view_iterator<basic_common_type, typename basic_common_type::reverse_iterator, sizeof...(Components) - 1u>;
  using iterable_view = iterable;

  template<typename Component>
  using storage_type = constness_as_t<Component, basic_storage<Entity, std::remove_const_t<Component>>>;

  basic_view() noexcept
  : _pools{},
    _view{} { }

  basic_view(storage_type<Components>&... components) noexcept
  : _pools{&components...},
    _view{_candidate()} { }

  template<typename Component>
  [[nodiscard]] basic_view use() const noexcept {
    auto other = basic_view{*this};
    other._view = std::get<storage_type<Component>*>(_pools);

    return other;
  }

  [[nodiscard]] size_type size_hint() const noexcept {
    return _view->size();
  }

  [[nodiscard]] iterator begin() const {
    return iterator{_view->begin(), _view->end(), _view->begin(), _test_set()};
  }

  [[nodiscard]] iterator end() const {
    return iterator{_view->begin(), _view->end(), _view->end(), _test_set()};
  }

  [[nodiscard]] reverse_iterator rbegin() const {
    return reverse_iterator{_view->rbegin(), _view->rend(), _view->rbegin(), _test_set()};
  }

  [[nodiscard]] reverse_iterator rend() const {
    return reverse_iterator{_view->rbegin(), _view->rend(), _view->rend(), _test_set()};
  }

  [[nodiscard]] entity_type front() const {
    const auto it = begin();
    return it != end() ? *it : null;
  }

  [[nodiscard]] entity_type back() const {
    const auto it = rbegin();
    return it != rend() ? *it : null;
  }

  [[nodiscard]] iterator find(const entity_type entity) const {
    const auto it = iterator{_view->begin(), _view->end(), _view->find(entity), _test_set()};
    return (it != end() && *it == entity) ? it : end();
  }

  [[nodiscard]] decltype(auto) operator[](const entity_type entity) const {
    return get<Components...>(entity);
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return _view != nullptr;
  }

  [[nodiscard]] bool contains(const entity_type entity) const {
    return (std::get<storage_type<Components>*>(_pools)->contains(entity) && ...);
  }

  template<typename... Component>
  [[nodiscard]] decltype(auto) get([[maybe_unused]] const entity_type entity) const {
    assert(contains(entity));

    if constexpr(sizeof...(Component) == 0) {
      return std::tuple_cat(std::get<storage_type<Component>*>(_pools)->get_as_tuple(entity)...);
    } else if constexpr(sizeof...(Component) == 1) {
      return (std::get<storage_type<Component>*>(_pools)->get(entity), ...);
    } else {
      return std::tuple_cat(std::get<storage_type<Component>*>(_pools)->get_as_tuple(entity)...);
    }
  }

  template<typename Function>
  void each(Function function) const {
    ((std::get<storage_type<Components>*>(_pools) == _view ? each<Components>(std::move(function)) : void()), ...);
  }

  template<typename Component, typename Function>
  void each(Function function) const {
    for(const auto current: iterable_storage<Entity, Component>{*std::get<storage_type<Component> *>(_pools)}) {
      if((is_multi_type_v || (std::get<0>(current) != tombstone))
        && ((std::is_same_v<Component, Components> || std::get<storage_type<Components> *>(_pools)->contains(std::get<0>(current))) && ...)) {
        if constexpr(is_applicable_v<Function, decltype(std::tuple_cat(std::tuple<entity_type>{}, std::declval<basic_view>().get({})))>) {
          std::apply(function, std::tuple_cat(std::make_tuple(std::get<0>(current)), _dispatch_get<Component, Components>(current)...));
        } else {
          std::apply(function, std::tuple_cat(_dispatch_get<Component, Components>(current)...));
        }
      }
    }
  }

  [[nodiscard]] iterable_view each() const noexcept {
    return iterable_view{*this};
  }

  template<typename Component>
  [[nodiscard]] iterable_view each() const noexcept {
    basic_view other{*this};
    other._view = std::get<storage_type<Component>*>(_pools);
    return iterable_view{std::move(other)};
  }

private:
  [[nodiscard]] const auto* _candidate() const noexcept {
    return (std::min)({static_cast<const basic_common_type*>(std::get<storage_type<Components>*>(_pools))...}, [](const auto* lhs, const auto* rhs) {
      return lhs->size() < rhs->size();
    });
  }

  [[nodiscard]] auto _test_set() const noexcept {
    auto position = size_type{};
    std::array<const basic_common_type*, sizeof...(Components) - 1u> other{};

    (static_cast<void>(std::get<storage_type<Components> *>(_pools) == _view ? void() : void(other[position++] = std::get<storage_type<Components> *>(_pools))), ...);

    return other;
  }

  template<typename Component, typename Other, typename... Args>
  [[nodiscard]] auto _dispatch_get(const std::tuple<Entity, Args...>& current) const {
    if constexpr(std::is_same_v<Component, Other>) {
      return std::forward_as_tuple(std::get<Args>(current)...);
    } else {
      return std::get<storage_type<Other>*>(_pools)->get_as_tuple(std::get<0>(current));
    }
  }

  template<typename, typename, typename>
  friend class basic_view;

  std::tuple<storage_type<Components>*...> _pools{};
  const basic_common_type* _view{};

};

template<typename Entity, typename Component>
class basic_view<Entity, type_list<Component>, std::void_t<std::enable_if_t<!in_place_delete_v<std::remove_const_t<Component>>>>> {

  using basic_common_type = typename basic_storage<Entity, std::remove_const_t<Component>>::base_type;

public:
  using entity_type = Entity;
  using size_type = std::size_t;
  using iterator = typename basic_common_type::iterator;
  using reverse_iterator = typename basic_common_type::reverse_iterator;
  using iterable_view = iterable_storage<Entity, Component>;
  using storage_type = constness_as_t<Component, basic_storage<Entity, std::remove_const_t<Component>>>;

  basic_view() noexcept
  : _pools{} { }

  basic_view(storage_type& storage) noexcept
  : _pools{&storage} { }

  [[nodiscard]] size_type size() const noexcept {
    return std::get<0>(_pools)->size();
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return std::get<0>(_pools)->is_empty();
  }

  [[nodiscard]] auto data() const noexcept {
      return std::get<0>(_pools)->data();
  }

  [[nodiscard]] iterator begin() const noexcept {
    return std::get<0>(_pools)->basic_common_type::begin();
  }

  [[nodiscard]] iterator end() const noexcept {
    return std::get<0>(_pools)->basic_common_type::end();
  }

  [[nodiscard]] reverse_iterator rbegin() const noexcept {
    return std::get<0>(_pools)->basic_common_type::rbegin();
  }

  [[nodiscard]] reverse_iterator rend() const noexcept {
    return std::get<0>(_pools)->basic_common_type::rend();
  }

  [[nodiscard]] entity_type front() const {
    const auto _current = begin();
    return _current != end() ? *_current : null;
  }

  [[nodiscard]] entity_type back() const {
    const auto _current = rbegin();
    return _current != rend() ? *_current : null;
  }

  [[nodiscard]] iterator find(const entity_type entity) const {
    const auto _current = std::get<0>(_pools)->find(entity);
    return _current != end() && *_current == entity ? _current : end();
  }

  [[nodiscard]] entity_type operator[](const size_type position) const {
    return begin()[position];
  }

  [[nodiscard]] decltype(auto) operator[](const entity_type entity) const {
    return get<Component>(entity);
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return std::get<0>(_pools) != nullptr;
  }

  [[nodiscard]] bool contains(const entity_type entity) const {
    return std::get<0>(_pools)->contains(entity);
  }

  template<typename... Components>
  [[nodiscard]] decltype(auto) get(const entity_type entity) const {
    assert(contains(entity));

    if constexpr(sizeof...(Components) == 0) {
      return std::get<0>(_pools)->get_as_tuple(entity);
    } else {
      static_assert(std::is_same_v<Components..., Component>, "Invalid component type");
      return std::get<0>(_pools)->get(entity);
    }
  }

  template<typename Function>
  void each(Function function) const {
    if constexpr(ignore_as_empty_v<std::remove_const_t<Component>>) {
      if constexpr(std::is_invocable_v<Function>) {
        for(auto position = size_type{}, _last = size(); position < _last; ++position) {
          function();
        }
      } else {
        for(auto entity : *this) {
          function(entity);
        }
      }
    } else {
      if constexpr(is_applicable_v<Function, decltype(*each().begin())>) {
          for(const auto pack : each()) {
            std::apply(function, pack);
          }
      } else {
        for(auto&& component: *std::get<0>(_pools)) {
          function(component);
        }
      }
    }
  }

  [[nodiscard]] iterable_view each() const noexcept {
    return iterable_view{*std::get<0>(_pools)};
  }

private:
  template<typename, typename, typename>
  friend class basic_view;

  std::tuple<storage_type*> _pools{};

};



} // namespace sbx

#endif // SBX_ECS_VIEW_HPP_
