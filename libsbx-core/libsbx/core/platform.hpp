#ifndef LIBSBX_CORE_PLATFORM_HPP_
#define LIBSBX_CORE_PLATFORM_HPP_

#if !defined(NDEBUG)
  #define LIBSBX_DEBUG
#endif

#if defined(WIN32) || defined(_WIN32)
  #define LIBSBX_PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define LIBSBX_PLATFORM_APPLE
#elif defined(__linux__) || defined(__linux)
  #define LIBSBX_PLATFORM_LINUX
#else 
  #error "Unsupported target platform"
#endif

#if defined(__clang__)
  #define LIBSBX_COMPILER_CLANG
#elif defined(__GNUC__)
  #define LIBSBX_COMPILER_GNU
#elif defined(__MSC_VER)
  #define LIBSBX_COMPILER_MSVC
#else
  #define LIBSBX_COMPILER_UNKOWN
#endif

#endif // LIBSBX_CORE_PLATFORM_HPP_
