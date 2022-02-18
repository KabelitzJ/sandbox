#ifndef SBX_PLATFORM_TARGTE_HPP_
#define SBX_PLATFORM_TARGTE_HPP_

#if defined(WIN32) || defined(_WIN32)
  #define SBX_PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define SBX_PLATFORM_APPLE
#elif defined(__linux__) || defined(__linux)
  #define SBX_PLATFORM_LINUX
#else 
  #error "Unsupported target platform"
#endif

#endif // SBX_PLATFORM_TARGTE_HPP_
