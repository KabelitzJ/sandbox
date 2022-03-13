#ifndef SBX_ECS_VIEW_HPP_
#define SBX_ECS_VIEW_HPP_

#include <vector>
#include <tuple>

#include "entity.hpp"

namespace sbx {

template<typename... Components>
class view {

  using container_type = std::vector<std::tuple<entity, Components&...>>;

public:

  using iterator = container_type::iterator;

  view() = default;

  view(const container_type& components)
  : _entries{components} { }

  ~view() = default;

  iterator begin() {
    return _entries.begin();
  }

  iterator end() {
    return _entries.end();
  }

private:

  container_type _entries{};

};

} // namespace sbx

#endif // SBX_ECS_VIEW_HPP_
