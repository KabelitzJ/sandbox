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
 * @file libsbx/core/module.hpp
 */

#ifndef LIBSBX_CORE_MODULE_HPP_
#define LIBSBX_CORE_MODULE_HPP_

/**
 * @ingroup libsbx-core
 */

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <libsbx/core/delegate.hpp>
#include <libsbx/core/type_name.hpp>
#include <libsbx/core/time.hpp>

namespace sbx::core {

template<typename... Types>
struct dependencies {
  static std::unordered_set<std::type_index> get() {
    static auto types = std::unordered_set<std::type_index>{};
    (types.insert(std::type_index{typeid(Types)}), ...);
    return types;
  }
};

class module_manager {

  template<typename>
  friend class module;

public:

  // Update stages for a module in execution order
  enum class stage : std::uint8_t {
    pre,
    normal,
    post,
    render,
  }; // enum class stage

  module_manager() = default;

  ~module_manager() = default;

  static void create_all() {
    for (const auto& entry : _factories) {
      const auto& type = entry.first;
      const auto& factory = entry.second;

      _create_module(type, factory);
    }
  }

  static void destroy_all() {
    for (auto& entry : _instances) {
      auto& type = entry.first;

      _destroy_module(type);
    }
  }

  static void update_stages(const time& delta_time) {
    update_stage(stage::pre, delta_time);
    update_stage(stage::normal, delta_time);
    update_stage(stage::post, delta_time);
    update_stage(stage::render, delta_time);
  }

  static void update_stage(stage stage, const time& delta_time) {
    if (const auto entry = _instances_by_stage.find(stage); entry != _instances_by_stage.cend()) {
      const auto& instance_types = entry->second;

      for (auto& type : instance_types) {
        auto& instance = _instances.at(type);

        instance->update(delta_time);
      }
    }
  }

private:

  class module_base;

  struct module_factory {
    stage module_stage{};
    std::unordered_set<std::type_index> dependencies{};
    delegate<std::unique_ptr<module_base>()> create_fn{};
  }; // struct module_factory

  class module_base {

  public:

    virtual ~module_base() = default;

    virtual void update(const time& delta_time) = 0;

  protected:

    static std::unordered_map<std::type_index, std::unique_ptr<module_base>>& instances() {
      return _instances;
    }

    static std::unordered_map<std::type_index, module_factory>& factories() {
      return _factories;
    }

  }; // struct module_base

  static void _create_module(const std::type_index& type, const module_factory& factory) {
    if (_instances.find(type) != _instances.cend()) {
      // Module has already been created.
      return;
    }

    for (const auto& dependency : factory.dependencies) {
      _create_module(dependency, _factories.at(dependency));
    }

    _instances.insert({type, std::invoke(factory.create_fn)});
    _instances_by_stage[factory.module_stage].push_back(type);
  }

  static void _destroy_module(const std::type_index& type) {
    if (_instances.find(type) == _instances.cend()) {
      // Module has already been destroyed.
      return;
    }

    for (const auto& dependency : _factories.at(type).dependencies) {
      _destroy_module(dependency);
    }

    _instances.at(type).reset();
  }

  inline static std::unordered_map<std::type_index, module_factory> _factories{};
  inline static std::unordered_map<std::type_index, std::unique_ptr<module_base>> _instances{};
  inline static std::unordered_map<stage, std::vector<std::type_index>> _instances_by_stage{};

}; // class module_manager

template<typename Derived>
class module : public module_manager::module_base {

  using module_factory = module_manager::module_factory;
  using module_base = module_manager::module_base; 

public:

  using stage = module_manager::stage;

  virtual ~module() {
    static_assert(!std::is_abstract_v<Derived>, "Derived class can not be abstract.");
    static_assert(std::is_base_of_v<module<Derived>, Derived>, "Invalid use of template parameter.");
  }

  static Derived& get() {
    const auto type = std::type_index{typeid(Derived)};

    auto& module_instances = instances();

    if (module_instances.find(type) == module_instances.cend()) {
      auto error_message = std::stringstream{};
      error_message << "Module of type '" << type_name(type) << "' has not been created yat. Check your dependencies.";

      throw std::runtime_error{error_message.str()};
    }

    return *static_cast<Derived*>(module_instances.at(type).get());
  }

protected:

  template<typename... Dependencies>
  static bool register_module(stage stage, dependencies<Dependencies...>&& dependencies = {}) {
    const auto type = std::type_index{typeid(Derived)};

    auto create_fn = []() -> std::unique_ptr<module_base> {
      return std::make_unique<Derived>();
    };

    auto factory = module_factory{
      .module_stage = stage,
      .dependencies = dependencies.get(),
      .create_fn = delegate<std::unique_ptr<module_base>()>{std::move(create_fn)}
    };

    const auto result = factories().insert({type, std::move(factory)}).second;

    if (!result) {
      throw std::runtime_error{"Module already has been registered"};
    }

    return true;
  }

}; // class module

} // namespace sbx::core

#endif // LIBSBX_CORE_MODULE_HPP_
