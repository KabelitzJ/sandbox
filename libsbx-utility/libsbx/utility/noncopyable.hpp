#ifndef LIBSBX_UTILITY_NONCOPYABLE_HPP_
#define LIBSBX_UTILITY_NONCOPYABLE_HPP_

namespace sbx::utility {

struct noncopyable {

  noncopyable() = default;

  noncopyable(const noncopyable&) = delete;

  noncopyable(noncopyable&&) = default;

  virtual ~noncopyable() = default;

  noncopyable& operator=(const noncopyable&) = delete;

  noncopyable& operator=(noncopyable&&) = default;

}; // struct noncopyable

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_NONCOPYABLE_HPP_
