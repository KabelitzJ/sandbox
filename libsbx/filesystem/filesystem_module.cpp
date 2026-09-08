// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/filesystem/filesystem_module.hpp>

#include <cstdlib>
#include <filesystem>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/target.hpp>
#include <libsbx/utility/profiler.hpp>
#include <libsbx/utility/exception.hpp>

#if defined(SBX_PLATFORM_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#elif defined(SBX_PLATFORM_APPLE)
  #include <cstdint>
  #include <mach-o/dyld.h>
#endif

#include <libsbx/filesystem/native_filesystem.hpp>

namespace sbx::filesystem {

auto executable_directory() -> std::filesystem::path {
#if defined(SBX_PLATFORM_WIN32)
  auto buffer = std::vector<wchar_t>(MAX_PATH);

  for (;;) {
    const auto size = ::GetModuleFileNameW(nullptr, buffer.data(), static_cast<::DWORD>(buffer.size()));

    if (size == 0) {
      throw std::runtime_error{"GetModuleFileNameW failed"};
    }

    if (size < buffer.size()) {
      return std::filesystem::path{std::wstring{buffer.data(), size}}.parent_path();
    }

    buffer.resize(buffer.size() * 2);
  }
#elif defined(SBX_PLATFORM_APPLE)
  auto buffer = std::vector<char>(1024);
  auto size = static_cast<std::uint32_t>(buffer.size());

  if (::_NSGetExecutablePath(buffer.data(), &size) != 0) {
    buffer.resize(size);
    if (::_NSGetExecutablePath(buffer.data(), &size) != 0) {
      throw std::runtime_error{"_NSGetExecutablePath failed"};
    }
  }

  return std::filesystem::canonical(std::filesystem::path{buffer.data()}).parent_path();
#else
  return std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
}

auto _has_marker(const std::filesystem::path& candidate) -> bool {
  auto ec = std::error_code{};
  return std::filesystem::exists(candidate / "manifest.txt", ec);
}

auto engine_data_directory() -> std::filesystem::path {
  static const auto root = []() -> std::filesystem::path {
    if (auto* env = std::getenv("SBX_DATA_DIR")) return env;

    // The build tree mirrors the installed layout, so there's exactly one candidate to check:
    // share/libsbx next to the executable's bin/ directory, dev or installed alike.
    if (auto candidate = executable_directory() / ".." / "share" / "libsbx"; _has_marker(candidate)) {
      return std::filesystem::canonical(candidate);
    }

    throw std::runtime_error{"engine data directory not found"};
  }();

  return root;
}

filesystem_module::filesystem_module()
: _filesystem{std::make_unique<virtual_filesystem>()} {
  // Every engine-internal file (shaders, dotnet assemblies, ...) is loaded through the
  // 'engine://' mount now, so a missing data directory is a hard startup failure, not a
  // degrade-with-a-warning -- there's nothing useful the engine can do without it.
  const auto data_directory = engine_data_directory();

  const auto mount = _filesystem->create_filesystem<native_filesystem>(alias{"engine://"}, data_directory.string());

  if (!mount) {
    throw utility::runtime_error{"Failed to register 'engine://' mount at '{}'", data_directory.string()};
  }

  utility::logger<"filesystem">::debug("Registered 'engine://' mount at '{}'", data_directory.string());
}

filesystem_module::~filesystem_module() {

}

auto filesystem_module::update() -> void {
  SBX_PROFILE_SCOPE("filesystem_module::update");
}

} // namespace sbx::filesystem
