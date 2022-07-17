#ifndef SBX_PLATFORM_ASSERT_HPP_
#define SBX_PLATFORM_ASSERT_HPP_

#include "target.hpp"

#if defined(SBX_DEBUG) && !defined(SBX_DISABLE_ASSERT)
  #include <cassert>
  #define SBX_ASSERT(condition, ...) assert(condition)
#else
  #undef SBX_ASSERT
  #define SBX_ASSERT(...) (void(0))
#endif

#endif // SBX_PLATFORM_ASSERT_HPP_
