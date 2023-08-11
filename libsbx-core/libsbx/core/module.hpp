#ifndef LIBSBX_CORE_MODULE_HPP_
#define LIBSBX_CORE_MODULE_HPP_

#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <memory>
#include <cinttypes>
#include <cmath>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::core {

template<typename Derived, typename Base>
concept derived_from = std::is_base_of_v<Base, Derived>;

class module_manager {

  template<typename>
  friend class module;

  friend class engine;

public:

  module_manager() = delete;
  
  ~module_manager() = default;

private:

  enum class stage : std::uint8_t {
    pre = 0,
    normal = 1,
    post = 2
  }; // enum class stage

  template<typename... Types>
  struct dependencies {
    auto get() const noexcept -> std::unordered_set<std::type_index> {
      auto types = std::unordered_set<std::type_index>{};
      (types.insert(std::type_index{typeid(Types)}), ...);
      return types;
    }
  }; // struct dependencies

  struct module_base {
    virtual ~module_base() = default;
    virtual auto update() -> void = 0;
  }; // struct module_base

  struct module_factory {
    module_manager::stage stage{};
    std::unordered_set<std::type_index> dependencies{};
    std::function<module_base*()> create{};
  }; // module_factory

  static auto _factories() -> std::unordered_map<std::type_index, module_factory>& {
    static auto instance = std::unordered_map<std::type_index, module_factory>{};
    return instance;
  }

}; // class module_manager

template<typename Derived>
class module : public module_manager::module_base, public utility::noncopyable {

public:

  virtual ~module() {
    static_assert(!std::is_abstract_v<Derived>, "Class may not be abstract.");
    static_assert(std::is_base_of_v<module<Derived>, Derived>, "Class must inherit from module<Class>.");
  }

protected:

  using base_type = module_manager::module_base;

  template<typename... Dependencies>
  using dependencies = module_manager::dependencies<Dependencies...>;

  using stage = module_manager::stage;

  template<derived_from<base_type>... Dependencies>
  static auto register_module(stage stage, dependencies<Dependencies...>&& dependencies = {}) -> bool {
    module_manager::_factories().insert({std::type_index{typeid(Derived)}, module_manager::module_factory{
      .stage = stage,
      .dependencies = dependencies.get(),
      .create = [](){
        auto* instance = reinterpret_cast<Derived*>(::operator new(sizeof(Derived)));

        if (!instance) {
          throw std::bad_alloc{};
        }

        std::construct_at(instance);

        return instance;
      }
    }});

    return true;
  }

}; // class module

} // namespace sbx::core

#endif // LIBSBX_CORE_MODULE_HPP_
