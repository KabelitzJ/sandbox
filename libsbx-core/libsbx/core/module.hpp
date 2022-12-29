#ifndef LIBSBX_CORE_MODULE_HPP_
#define LIBSBX_CORE_MODULE_HPP_

#include <type_traits>
#include <unordered_set>
#include <typeindex>

namespace sbx::core {

template<typename... Args>
struct dependencies {
  std::unordered_set<std::type_index> get() const noexcept {

  }
}; // struct dependencies

class module_manager {

  template<typename>
  friend class module;

public:

private:

}; // class module_manager

template<typename Type>
class module {

public:

  virtual ~module() {
    static_assert(!std::is_abstract_v<Type>, "");
    static_assert(std::is_base_of_v<module<Type>, Type>, "");

    if (static_cast<Type*>(this) == _instance) {
      _instance = nullptr;
    }
  }

private:

  static Type* _instance{nullptr};

}; // class module

} // namespace sbx::core

#endif // LIBSBX_CORE_MODULE_HPP_
