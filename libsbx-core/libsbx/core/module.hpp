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

namespace sbx::core {

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
    virtual auto update(std::float_t delta_time) -> void = 0;
  }; // struct module_base

  struct module_factory {
    stage stage{};
    std::unordered_set<std::type_index> dependencies{};
    std::function<std::unique_ptr<module_base>()> create{};
  }; // module_factory

  inline static std::unordered_map<std::type_index, module_factory> _factories{};

}; // class module_manager

template<typename Type>
class module : public module_manager::module_base {

public:

  virtual ~module() {
    static_assert(!std::is_abstract_v<Type>, "Class may not be abstract.");
    static_assert(std::is_base_of_v<module<Type>, Type>, "Class must inherit from module<Class>.");

    if (static_cast<Type*>(this) == _instance) {
      _instance = nullptr;
    }
  }

  static auto get() noexcept -> Type& {
    return *_instance;
  }

protected:

  template<typename... Types>
  using dependencies = module_manager::dependencies<Types...>;

  using stage = module_manager::stage;

  template<typename... Types>
  static auto register_module(stage stage, dependencies<Types...>&& dependencies = {}) -> bool {
    module_manager::_factories.insert({std::type_index{typeid(Type)}, module_manager::module_factory{
      .stage = stage,
      .dependencies = dependencies.get(),
      .create = [](){
        auto instance = std::make_unique<Type>();
        module<Type>::_instance = instance.get();
        return instance;
      }
    }});

    return true;
  }

private:

  inline static Type* _instance{};

}; // class module

} // namespace sbx::core

#endif // LIBSBX_CORE_MODULE_HPP_
