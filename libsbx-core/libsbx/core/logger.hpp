/* 
 * Copyright (c) 2022 Jonas Kabelitz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT/>.
 */

/**
 * @file libsbx/core/logger.hpp
 */

#ifndef LIBSBX_CORE_LOGGER_HPP_
#define LIBSBX_CORE_LOGGER_HPP_

/**
 * @ingroup libsbx-core
 */

#include <memory>
#include <source_location>
#include <unordered_map>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <libsbx/utility/target.hpp>

namespace sbx::core {

class logger {

public:

  template<typename... Args>
  using format_string_type = spdlog::format_string_t<Args...>;

  logger() = delete;

  ~logger() = default;

  template<typename... Args>
  static auto trace(std::string name, format_string_type<Args...> format, Args&&... args) -> void {
    // [NOTE] KAJ 2023-03-20 19:43 - This should make trace and debug messages be no-ops in release builds.
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance(std::move(name)).trace(format, std::forward<Args>(args)...);
    }
  }

  template<typename Type>
  static auto trace(std::string name, const Type& value) -> void {
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance(std::move(name)).trace(value);
    }
  }

  template<typename... Args>
  static auto debug(std::string name, format_string_type<Args...> format, Args&&... args) -> void {
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance(std::move(name)).debug(format, std::forward<Args>(args)...);
    }
  }

  template<typename Type>
  static auto debug(std::string name, const Type& value) -> void {
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance(std::move(name)).debug(value);
    }
  }

  template<typename... Args>
  static auto info(std::string name, format_string_type<Args...> format, Args&&... args) -> void {
    _instance(std::move(name)).info(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto info(std::string name, const Type& value) -> void {
    _instance(std::move(name)).info(value);
  }

  template<typename... Args>
  static auto warn(std::string name, format_string_type<Args...> format, Args&&... args) -> void {
    _instance(std::move(name)).warn(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto warn(std::string name, const Type& value) -> void {
    _instance(std::move(name)).warn(value);
  }

  template<typename... Args>
  static auto error(std::string name, format_string_type<Args...> format, Args&&... args) -> void {
    _instance(std::move(name)).error(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto error(std::string name, const Type& value) -> void {
    _instance(std::move(name)).error(value);
  }

  template<typename... Args>
  static auto critical(std::string name, format_string_type<Args...> format, Args&&... args) -> void {
    _instance(std::move(name)).critical(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto critical(std::string name, const Type& value) -> void {
    _instance(std::move(name)).critical(value);
  }

private:

  static auto _instance(std::string name) -> spdlog::logger& {
    if (auto entry = _loggers.find(name); entry != _loggers.end()) {
      return entry->second;
    }

    auto logger = _create_logger(name);

    auto entry = _loggers.emplace(std::move(name), std::move(logger));

    return entry.first->second;
  }

  static auto _create_logger(std::string name) -> spdlog::logger {
    auto sinks = std::vector<std::shared_ptr<spdlog::sinks::sink>>{};

    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("./demo/logs/sbx.log", true));

    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    auto logger = spdlog::logger{std::move(name), std::begin(sinks), std::end(sinks)};

    logger.set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] : %v");

    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      logger.set_level(spdlog::level::debug);
    } else {
      logger.set_level(spdlog::level::info);
    }

    return logger;
  } 

  inline static std::unordered_map<std::string, spdlog::logger> _loggers{};

}; // class logger

} // namespace sbx::core

#endif // LIBSBX_CORE_LOGGER_HPP_
