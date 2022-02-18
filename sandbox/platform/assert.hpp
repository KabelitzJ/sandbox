#ifndef SBX_PLATFORM_ASSERT_HPP_
#define SBX_PLATFORM_ASSERT_HPP_

#if defined DEBUG && !SBX_DISABLE_ASSERT
  #undef SBX_ASSERT
  #define SBX_ASSERT(...) (void(0))
#else
  #include <cassert>
  #define SBX_ASSERT(condition, ...) assert(condition)
#endif

#endif // SBX_PLATFORM_ASSERT_HPP_
