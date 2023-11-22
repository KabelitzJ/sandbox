#ifndef LIBSBX_CORE_CLI_HPP_
#define LIBSBX_CORE_CLI_HPP_

#include <string>
#include <vector>
#include <string_view>
#include <concepts>
#include <cinttypes>
#include <optional>

namespace sbx::core {

class argument_base {

public:

  argument_base(const std::string& name, const std::string& description)
  : _name{name},
    _description{description} { }

  virtual ~argument_base() = default;

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  auto description() const noexcept -> const std::string& {
    return _description;
  }

protected:

  std::string _name;
  std::string _description;

}; // class argument_base

template<typename Type>
class argument : public argument_base {

  friend class cli;

public:

  argument(const std::string& name, const std::string& description)
  : argument_base{name, description} { }

  ~argument() = default;

  auto has_value() const noexcept -> bool {
    return _value.has_value();
  }

  auto value() const noexcept -> const Type& {
    return _value.value();
  }

private:

  template<typename... Args>
  requires (std::constructible_from<Type, Args...>)
  auto _set_value(Args&&... args) noexcept -> void {
    _value.emplace(std::forward<Args>(args)...);
  }

  std::optional<Type> _value;

}; // class argument

class cli {

  friend class engine;

public:

  cli(std::vector<std::string_view>&& args)
  : _args{std::move(args)} { }

  template<typename Type>
  auto add_argument(const std::string& name, const std::string& description = "") -> argument<Type>& {
    auto result = _arguments.emplace(name, std::make_unique<argument<Type>>(name, description));

    return *static_cast<argument<Type>*>(result.first->second.get());
  }

private:

    auto _parse(std::vector<std::string_view>&& args) -> void {
      static_cast<void>(args);
    }

  std::vector<std::string_view> _args;
  std::unordered_map<std::string, std::unique_ptr<argument_base>> _arguments;

}; // class cli

} // namespace sbx::core

#endif // LIBSBX_CORE_CLI_HPP_
