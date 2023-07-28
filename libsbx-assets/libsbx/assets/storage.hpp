#ifndef LIBSBX_ASSETS_STORAGE_HPP_
#define LIBSBX_ASSETS_STORAGE_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>

namespace sbx::assets {

struct storage_base {
  virtual ~storage_base() = default;
}; // class storage_base

template<typename Type>
class storage : public storage_base {

public:

private:

}; // class storage

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_STORAGE_HPP_
