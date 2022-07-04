#include "view.hpp"

namespace sbx {

template<component... Components>
inline view<Components...>::view(view&& other) noexcept
: _entries{std::move(other._entries)} { }

template<component... Components>
inline view<Components...>& view<Components...>::operator=(view&& other) noexcept {
  if (this != &other) {
    _entries = std::move(other._entries);
  }

  return *this;
}

template<component... Components>
inline view<Components...>::iterator view<Components...>::begin() noexcept {
  return _entries.begin();
}

template<component... Components>
inline view<Components...>::const_iterator view<Components...>::begin() const noexcept {
  return _entries.begin();
}

template<component... Components>
inline view<Components...>::const_iterator view<Components...>::cbegin() const noexcept {
  return _entries.cbegin();
}

template<component... Components>
inline view<Components...>::iterator view<Components...>::end() noexcept {
  return _entries.end();
}

template<component... Components>
inline view<Components...>::const_iterator view<Components...>::end() const noexcept {
  return _entries.end();
}

template<component... Components>
inline view<Components...>::const_iterator view<Components...>::cend() const noexcept {
  return _entries.cend();
}

template<component... Components>
inline view<Components...>::size_type view<Components...>::size() const noexcept {
  return _entries.size();
}

template<component... Components>
inline bool view<Components...>::empty() const noexcept {
  return _entries.empty();
}

template<component... Components>
inline view<Components...>::view(const container_type& components)
: _entries{components} { }

} // namespace sbx
