#ifndef LIBSBX_CORE_APPLICATION_HPP_
#define LIBSBX_CORE_APPLICATION_HPP_

#include <cinttypes>
#include <utility>

#include <libsbx/core/version.hpp>

namespace sbx::core {

struct version {
  std::uint8_t major{};
  std::uint8_t minor{};
  std::uint8_t patch{};
}; // struct version

class application {

public:


  application(version&& version)
  : _version{std::move(version)} { }

  virtual ~application() = default;

  virtual auto update() -> void = 0;

  const version& version() const noexcept {
    return _version;
  }

private:

  core::version _version{};

}; // class application

} // namespace sbx::core

#endif // LIBSBX_CORE_APPLICATION_HPP_
