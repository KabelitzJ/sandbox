#include <iostream>
#include <cstdlib>
#include <chrono>

#include <libsbx/core/core.hpp>

class foo : public sbx::core::module<foo> {

  inline static const auto is_registered = register_module(stage::normal);

public:

  foo() {
    sbx::core::logger::info("foo::foo()");
  }

  ~foo() override {
    sbx::core::logger::info("foo::~foo()");
  }

  auto update(std::float_t delta_time) -> void override {
    sbx::core::logger::info("foo::update({})", delta_time);
  }

  auto value() const noexcept -> std::uint32_t {
    return _value;
  }

private:

  std::uint32_t _value{42};

}; // class foo

class bar : public sbx::core::module<bar> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<foo>{});

public:

  bar() {
    sbx::core::logger::info("bar::bar()");
    sbx::core::logger::info("foo::value: {}", foo::get().value());
    sbx::core::logger::info("bar::value: {}", _value);
  }

  ~bar() override {
    sbx::core::logger::info("bar::~bar()");
  }

  auto update(std::float_t delta_time) -> void override {
    sbx::core::logger::info("bar::update({})", delta_time);
  }

  auto value() const noexcept -> std::uint32_t {
    return _value;
  }

private:

  std::uint32_t _value{69};

}; // class bar

auto main(int argc, char** argv) -> int {
  sbx::core::logger::info("Hello, World!");

  try {
    auto engine = std::make_unique<sbx::core::engine>(std::vector<std::string_view>{argv, argv + argc});
    engine->run();
  } catch(const std::exception& exception) {
    sbx::core::logger::error("{}", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
