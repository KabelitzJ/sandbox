#ifndef SBX_ECS_VIEW_HPP_
#define SBX_ECS_VIEW_HPP_

#include <vector>
#include <tuple>

#include "entity.hpp"

namespace sbx {

template<typename... Components>
class view final {

  friend class registry;

  using underlying_type = std::tuple<const entity&, Components&...>;
  using container_type = std::vector<underlying_type>;

public:

  using value_type = underlying_type;
  using size_type = container_type::size_type;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  view() = default;

  ~view() = default;

  [[nodiscard]] iterator begin() {
    return _entries.begin();
  }

  [[nodiscard]] const_iterator begin() const {
    return _entries.begin();
  }

  [[nodiscard]] const_iterator cbegin() const {
    return _entries.cbegin();
  }

  [[nodiscard]] iterator end() {
    return _entries.end();
  }

  [[nodiscard]] const_iterator end() const {
    return _entries.end();
  }

  [[nodiscard]] const_iterator cend() const {
    return _entries.cend();
  }

  [[nodiscard]] size_type size() const {
    return _entries.size();
  }

  [[nodiscard]] bool empty() const {
    return _entries.empty();
  }

private:

  view(const container_type& components)
  : _entries{components} { }

  container_type _entries{};

};

} // namespace sbx

#endif // SBX_ECS_VIEW_HPP_
