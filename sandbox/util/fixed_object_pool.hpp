#ifndef SBX_GFX_OBJECT_POOL_HPP_
#define SBX_GFX_OBJECT_POOL_HPP_

#include <cinttypes>

namespace sbx {

// (TODO) KAJ 17.09.2021 Fix major bug here
class fixed_object_pool {

public:
  fixed_object_pool(std::size_t size, std::size_t element_size);
  ~fixed_object_pool();

  template<typename T, typename... Args>
  T* construct(std::size_t index, Args&&... args);

  template<typename T>
  void destroy(std::size_t index);

  template<typename T>
  T* get(std::size_t index);

private:
  std::size_t _element_size;
  std::uint8_t* _buffer;

}; // class object_pool

} // namespace sbx

#endif // SBX_GFX_OBJECT_POOL_HPP_
