#ifndef LIBSBX_CORE_CLI_HPP_
#define LIBSBX_CORE_CLI_HPP_

#include <string>
#include <vector>
#include <string_view>
#include <concepts>
#include <cinttypes>
#include <optional>
#include <charconv>

#include <fmt/format.h>

namespace sbx::core {

template<typename Type>
concept argument = (std::is_same_v<Type, bool> || std::is_integral_v<Type> || std::is_floating_point_v<Type> || std::is_same_v<Type, std::string>);

class cli {

  friend class engine;

public:

  cli(std::vector<std::string>&& args)
  : _args{std::move(args)} { }

  template<argument Type>
  auto argument(const std::string& name) -> std::optional<Type> {
    auto value = _argument_value(name);

    if (!value) {
      return std::nullopt;
    }

    if constexpr (std::is_same_v<Type, bool>) {
      if (*value == "true") {
        return true;
      } else if (*value == "false") {
        return false;
      } else {
        return std::nullopt;
      }
    } else if constexpr (std::is_floating_point_v<Type> || std::is_integral_v<Type>) {
      auto result = Type{};

      auto [ptr, ec] = std::from_chars(value->data(), value->data() + value->size(), result);

      if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
        return std::nullopt;
      }

      return result;
    } else if constexpr (std::is_same_v<Type, std::string>) {
      return value;
    }
  }

private:

  auto _argument_value(const std::string& name) -> std::optional<std::string> {
    if (auto entry = _argument_cache.find(name); entry != _argument_cache.end()) {
      return entry->second;
    }

    const auto key = fmt::format("--{}=", name);

    const auto value_position = std::ranges::find_if(_args, [&key](const auto& arg) { return arg.starts_with(key); });

    if (value_position == _args.end()) {
      return std::nullopt;
    }

    const auto value = (*value_position).substr(key.size());

    _argument_cache[name] = value;

    return value;
  }

  std::vector<std::string> _args;
  std::unordered_map<std::string, std::string> _argument_cache;

}; // class cli

} // namespace sbx::core

#endif // LIBSBX_CORE_CLI_HPP_
