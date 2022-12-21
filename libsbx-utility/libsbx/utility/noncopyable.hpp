#ifndef LIBSBX_UTILITY_NONCOPYABLE_HPP_
#define LIBSBX_UTILITY_NONCOPYABLE_HPP_

namespace sbx::utility {

struct noncopyable {
  noncopyable() = default;
  noncopyable(const noncopyable& other) = delete;
  noncopyable(noncopyable&& other) = default;
  virtual ~noncopyable() = default;
  noncopyable& operator=(const noncopyable& other) = delete;
  noncopyable& operator=(noncopyable&& other) = default;
}; // class noncopyable

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_NONCOPYABLE_HPP_
