/* 
 * Copyright (c) 2022 Jonas Kabelitz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT/>.
 */

/**
 * @file libsbx/ecs/registry.hpp
 */

#ifndef LIBSBX_ECS_REGISTRY_HPP_
#define LIBSBX_ECS_REGISTRY_HPP_

/**
 * @ingroup libsbx-ecs
 */

#include <memory>
#include <ranges>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <libsbx/ecs/entity.hpp>

namespace sbx::ecs {

namespace detail {

struct component_container_base {
  virtual ~component_container_base() = default;
  virtual void erase(const entity& entity) = 0;
  virtual std::size_t size() const noexcept = 0;
  virtual bool is_empty() const noexcept = 0;
  virtual void clear() = 0;
}; // class component_container_base

template<typename Type> 
class component_container : public component_container_base {

  using sparse_container_type = std::unordered_map<entity, std::size_t>;
  using dense_container_type = std::vector<entity>;
  using component_container_type = std::vector<Type>;

public:

  using iterator = component_container_type::iterator;

  component_container() = default;

  ~component_container() override = default;

  template<typename... Args>
  Type& insert(const entity& entity, Args&&... args) {
    if (const auto entry = _sparse.find(entity); entry != _sparse.cend()) {
      return _components.at(entry->second);
    } else {
      const auto index = _components.size();

      if constexpr (std::is_aggregate_v<Type>) {
        _components.push_back(Type{std::forward<Args>(args)...});
      } else {
        _components.emplace_back(std::forward<Args>(args)...);
      }

      _sparse.insert({entity, index});
      _dense.push_back(entity);

      return _components.back();
    }
  }

  void erase(const entity& entity) override {
    if (const auto entry = _sparse.find(entity); entry != _sparse.cend()) {
      const auto index = entry->second;

      _sparse.at(_dense.back()) = index;

      std::swap(_dense.at(index), _dense.back());
      _dense.pop_back();

      std::swap(_components.at(index), _components.back());
      _components.pop_back();

      _sparse.erase(entity);
    }
  }

  iterator begin() {
    return _components.begin();
  }

  iterator end() {
    return _components.end();
  }

  iterator find(const entity& entity) {
    if (auto entry = _sparse.find(entity); entry != _sparse.end()) {
      return begin() + entry->second;
    }

    return end();
  }

  Type& at(const entity& entity) {
    return _components.at(_sparse.at(entity));
  }

  bool contains(const entity& entity) {
    return _sparse.contains(entity);
  }

  std::size_t size() const noexcept override {
    return _components.size();
  }

  bool is_empty() const noexcept override {
    return _components.empty();
  }

  void clear() override {
    _dense.clear();
    _sparse.clear();
    _components.clear();
  }

private:

  sparse_container_type _sparse{};
  dense_container_type _dense{};
  component_container_type _components{};

}; // class component_container

} // namespace detail

template<typename... Types>
class view {

  friend class registry;

  using container_type = std::vector<std::tuple<const entity&, Types...>>;

public:

  using iterator = container_type::iterator;

  view() = default;

  iterator begin() {
    return _values.begin();
  }

  iterator end() {
    return _values.end();
  }

private:

  view(container_type&& values)
  : _values(std::forward<container_type>(values)) { }

  container_type _values{};

}; // class view

class tag {

public:

  tag(const std::string& value) : _value{value} { }

  tag(std::string&& value) : _value{std::move(value)} { }

  ~tag() = default;

  operator std::string() const noexcept {
    return _value;
  }

  operator std::string_view() const noexcept {
    return std::string_view{_value.c_str(), _value.size()};
  }

private:

  std::string _value{};

}; // class tag

template<typename OutputStream>
OutputStream& operator<<(OutputStream& output_stream, const tag& tag) {
  return output_stream << std::string_view{tag};
}

class registry {

  template<typename Type>
  using container_type = detail::component_container<std::remove_const_t<Type>>;

public:

  entity create_entity(const std::string& name = "Entity") {
    if (!_free_entities.empty()) {
      const auto index = _free_entities.back();
      _free_entities.pop_back();

      auto entity = _entities.at(index);
      add_component<tag>(entity, name);
      return entity;
    }

    const auto id = static_cast<entity::id_type>(_entities.size());

    _entities.push_back(entity{id, entity::version_type{0}});

    auto entity = _entities.back();
    add_component<tag>(entity, name);
    return entity;
  }

  void destroy_entity(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Entity is not valid"};
    }

    for (const auto& [type, containers] : _containers) {
      containers->erase(entity);
    }

    const auto index = static_cast<std::size_t>(entity._id());

    _entities[index]._increment_version();
    
    _free_entities.push_back(index);
  }

  bool is_valid_entity(const entity& entity) const noexcept {
  if (entity == entity::null) {
    return false;
  }

  const auto index = static_cast<std::size_t>(entity._id());

  return index < _entities.size() && _entities[index] == entity;
}

  template<typename Type, typename... Args>
  Type& add_component(const entity& entity, Args&&... args) {
    return _assure<Type>().insert(entity, std::forward<Args>(args)...);
  }

  template<typename Type>
  void remove_component(const entity& entity) {
    _assure<Type>().erase(entity);
  }

  template<typename Type>
  bool has_component(const entity& entity) {
    return _assure<Type>().contains(entity);
  }

  template<typename Type>
  Type& get_component(const entity& entity) {
    return _assure<Type>().at(entity);
  }

  template<typename... Types>
  view<Types...> create_view() {
    if constexpr (sizeof...(Types) == 0) {
      return view{};
    }

    auto has_component_filter = [this](const auto& entity){
      const auto has_components = std::initializer_list{has_component<Types>(entity)...};
      return std::all_of(has_components.begin(), has_components.end(), std::identity{});
    };

    auto values = std::vector<std::tuple<const entity&, Types...>>{};

    for (const auto& entity : _entities | std::views::filter(has_component_filter)) {
      values.emplace_back(std::forward_as_tuple(entity, get_component<Types>(entity)...));
    }

    return view<Types...>{std::move(values)};
  }

private:

  template<typename Type>
  container_type<Type>& _assure() {
  const auto type = std::type_index{typeid(Type)};

  auto entry = _containers.find(type);

  if (entry == _containers.cend()) {
    entry = _containers.insert({type, std::make_unique<container_type<Type>>()}).first;
  }

    return *static_cast<container_type<Type>*>(entry->second.get());
  }

  std::unordered_map<std::type_index, std::unique_ptr<detail::component_container_base>> _containers{};
  std::vector<entity> _entities{};
  std::vector<std::size_t> _free_entities{};

}; // class registry

} // namespace sbx::ecs

#endif // LIBSBX_ECS_REGISTRY_HPP_
