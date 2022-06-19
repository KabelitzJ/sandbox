#ifndef SBX_UTILS_NONMOVABLE_HPP_
#define SBX_UTILS_NONMOVABLE_HPP_

namespace sbx {

class nonmovable {

protected:

  constexpr nonmovable() noexcept = default;

  virtual ~nonmovable() noexcept = default;

  nonmovable(nonmovable&&) = delete;

  nonmovable& operator=(nonmovable&&) = delete;

}; // class nonmovable

} // namespace sbx

#endif // SBX_UTILS_NONMOVABLE_HPP_
