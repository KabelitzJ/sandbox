#ifndef SBX_UTILS_NONCOPYABLE_HPP_
#define SBX_UTILS_NONCOPYABLE_HPP_

namespace sbx {

class noncopyable {

protected:

  constexpr noncopyable() noexcept = default;

  virtual ~noncopyable() noexcept = default;

  noncopyable(const noncopyable&) = delete;

  noncopyable& operator=(const noncopyable&) = delete;

}; // class noncopyable

} // namespace sbx

#endif // SBX_UTILS_NONCOPYABLE_HPP_
