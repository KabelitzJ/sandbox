#ifndef SANDBOX_RESOURCE_HANDLE_HPP_
#define SANDBOX_RESOURCE_HANDLE_HPP_

namespace sbx {

template<typename T>
class resource_pool;

template<typename T>
class resource_handle {

public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  resource_handle(const resource_handle<T>&) = delete;
  resource_handle(resource_handle<T>&&) = default;
  ~resource_handle();

  resource_handle<T>& operator=(const resource_handle<T>&) = delete;
  resource_handle<T>& operator=(resource_handle<T>&&) = default;

  [[nodiscard]] reference operator*();
  [[nodiscard]] const_reference operator*() const;

  [[nodiscard]] pointer operator->();
  [[nodiscard]] const_pointer operator->() const;

private:
  resource_handle(resource_pool<T>* parent, pointer resource);
  void _invalidate_parent();

  resource_pool<T>* _parent;
  pointer _resource;
  bool _is_parent_valid;

  template<typename>
  friend class resource_pool;

};

template<typename T>
resource_handle<T>::~resource_handle() {
  if (_is_parent_valid) {
    _parent->_release(_resource);
    _resource = nullptr;
  } else {
    _resource->~T();
    ::operator delete(static_cast<void*>(_resource));
    _resource = nullptr;
  }
}

template<typename T>
typename resource_handle<T>::reference resource_handle<T>::operator*() {
  return *_resource;
}

template<typename T>
typename resource_handle<T>::const_reference resource_handle<T>::operator*() const {
  return *_resource;
}

template<typename T>
typename resource_handle<T>::pointer resource_handle<T>::operator->() {
  return _resource;
}

template<typename T>
typename resource_handle<T>::const_pointer resource_handle<T>::operator->() const {
  return _resource;
}

template<typename T>
resource_handle<T>::resource_handle(resource_pool<T>* parent, pointer resource) : _parent(parent), _resource(resource), _is_parent_valid(true) {

}

template<typename T>
void resource_handle<T>::_invalidate_parent() {
  _is_parent_valid = false;
}


} // namespace sbx

#endif // SANDBOX_RESOURCE_HANDLE_HPP_
