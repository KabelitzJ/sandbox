#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <concepts>

#include <types/primitives.hpp>

namespace sbx {

template<std::unsigned_integral Type>
class basic_registry {

public:

  using id_type = Type;

  constexpr basic_registry() noexcept;

  constexpr basic_registry(const basic_registry<id_type>& other) noexcept = delete;

  constexpr basic_registry(basic_registry<id_type>&& other) noexcept;

  ~basic_registry() noexcept;

  constexpr basic_registry<id_type>& operator=(const basic_registry<id_type>& other) noexcept = delete;

  constexpr basic_registry<id_type>& operator=(basic_registry<id_type>&& other) noexcept;

private:

}; // class basic_registry

using registry = basic_registry<uint32>;

} // namespace sbx

#include "registry.inl"

#endif // SBX_ECS_REGISTRY_HPP_
