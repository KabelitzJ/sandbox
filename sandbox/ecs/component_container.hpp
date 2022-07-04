#ifndef SBX_ECS_COMPONENT_CONTAINER_HPP_
#define SBX_ECS_COMPONENT_CONTAINER_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <concepts>

#include "entity.hpp"

namespace sbx {

class component_container_base {

public:

  virtual ~component_container_base() = default;

  virtual void remove(const entity& entity) = 0;

  virtual bool contains(const entity& entity) const noexcept = 0;

}; // class component_container_base

template<typename Type>
concept component = 
  !std::is_abstract_v<Type> &&
  !std::is_void_v<Type> && 
  !std::is_reference_v<Type> &&
  !std::is_volatile_v<Type> && 
  std::is_standard_layout_v<Type>;

template<component Type, typename Allocator = std::allocator<Type>>
class component_container : public component_container_base {

  using allocator_traits = std::allocator_traits<Allocator>;

public:

  using value_type = Type;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using allocator_type = Allocator;

  component_container() noexcept;

  component_container(const component_container& other) = delete;

  component_container(component_container&& other) noexcept;

  ~component_container() override = default;

  component_container& operator=(const component_container& other) = delete;

  component_container& operator=(component_container&& other) noexcept;

  void remove(const entity& entity) override;

  [[nodiscard]] bool contains(const entity& entity) const noexcept override;

  template<typename... Args>
  requires std::constructible_from<Type, Args...>
  reference add(const entity& entity, Args&&... args);

  [[nodiscard]] const_reference get(const entity& entity) const;

  [[nodiscard]] reference get(const entity& entity);

  template<std::invocable<Type&> Function>
  void patch(const entity& entity, Function&& function);

private:

  class component_deleter {

  public:

    component_deleter(allocator_type& allocator);

    ~component_deleter() = default;

    void operator()(value_type* component);

  private:

    allocator_type* _allocator{};

  };

  allocator_type _allocator{};
  std::unordered_map<entity, size_type> _sparse{};
  std::vector<entity> _dense{};
  std::vector<std::unique_ptr<value_type, component_deleter>> _components{};

}; // class component_container

} // namespace sbx

#include "component_container.inl"

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
