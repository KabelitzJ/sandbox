#ifndef SBX_ECS_VIEW_HPP_
#define SBX_ECS_VIEW_HPP_

#include <vector>
#include <tuple>

#include "entity.hpp"
#include "component_container.hpp"

namespace sbx {

template<component... Components>
class view final {

  friend class registry;

  using underlying_type = std::tuple<const entity&, Components&...>;
  using container_type = std::vector<underlying_type>;

public:

  using value_type = underlying_type;
  using size_type = container_type::size_type;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  view(const view& other) = delete;

  view(view&& other) noexcept;

  ~view() = default;

  view& operator=(const view& other) = delete;

  view& operator=(view&& other) noexcept;

  [[nodiscard]] iterator begin() noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] iterator end() noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;

  [[nodiscard]] size_type size() const noexcept;

  [[nodiscard]] bool empty() const noexcept;

private:

  view() = default;

  view(const container_type& components);

  container_type _entries{};

}; // class view

} // namespace sbx

#include "view.inl"

#endif // SBX_ECS_VIEW_HPP_
