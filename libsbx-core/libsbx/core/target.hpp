#ifndef LIBSBX_CORE_TARGET_HPP_
#define LIBSBX_CORE_TARGET_HPP_

#if !defined(NDEBUG) || defined(DEBUG)
  #define SBX_DEBUG
#endif

#if defined(WIN32) || defined(_WIN32)
  #define SBX_PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define SBX_PLATFORM_APPLE
#elif defined(__linux__) || defined(__linux)
  #define SBX_PLATFORM_LINUX
#else 
  #error "Unsupported target platform"
#endif

#if defined(__clang__)
  #define SBX_COMPILER_CLANG
#elif defined(__GNUC__)
  #define SBX_COMPILER_GNUC
#elif defined(__MSC_VER)
  #define SBX_COMPILER_MSC
#else
  #define SBX_COMPILER_UNKOWN
#endif

#endif // LIBSBX_CORE_TARGET_HPP_
