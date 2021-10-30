#ifndef SBX_UTILS_REVERSE_ADAPTOR_HPP_
#define SBX_UTILS_REVERSE_ADAPTOR_HPP_

#include <type_traits>
#include <iterator>

namespace sbx {

template<typename Container>
class reverse_adaptor {

public:

  using container_type = Container;
  using iterator = typename container_type::reverse_iterator;
  using const_iterator = typename container_type::const_reverse_iterator;

  explicit reverse_adaptor(container_type& container)
  : _container{container} { }

  iterator begin() {
    return _container.rbegin();
  }

  const_iterator begin() const {
    return _container.crbegin();
  }

  const_iterator cbegin() const {
    return _container.crbegin();
  }

  iterator end() {
    return _container.rend();
  }

  const_iterator end() const {
    return _container.crend();
  }

  const_iterator cend() const {
    return _container.crend();
  }

private:

  container_type& _container{};

};

} // namespace sbx

#endif // SBX_UTILS_REVERSE_ADAPTOR_HPP_
