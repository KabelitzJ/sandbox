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

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <libsbx/core/target.hpp>

namespace sbx::core {

class logger {

public:

  template<typename... Args>
  using format_string_type = spdlog::format_string_t<Args...>;

  logger() = delete;

  ~logger() = default;

  template<typename... Args>
  static auto trace(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().trace(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto trace(const Type& value) -> void {
    _instance().trace(value);
  }

  template<typename... Args>
  static auto debug(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().debug(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto debug(const Type& value) -> void {
    _instance().debug(value);
  }

  template<typename... Args>
  static auto info(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().info(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto info(const Type& value) -> void {
    _instance().info(value);
  }

  template<typename... Args>
  static auto warn(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().warn(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto warn(const Type& value) -> void {
    _instance().warn(value);
  }

  template<typename... Args>
  static auto error(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().error(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto error(const Type& value) -> void {
    _instance().error(value);
  }

  template<typename... Args>
  static auto critical(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().critical(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto critical(const Type& value) -> void {
    _instance().critical(value);
  }

private:

  static auto _instance() -> spdlog::logger& {
    static auto instance = _create_logger();
    return instance;
  }

  static auto _create_logger() -> spdlog::logger {
    auto logger = spdlog::logger{"sbx", std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};

    logger.set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] : %v");

    if constexpr (build_configuration_v == build_configuration::debug) {
      logger.set_level(spdlog::level::debug);
    } else {
      logger.set_level(spdlog::level::info);
    }

    return logger;
  } 

}; // class logger

} // namespace sbx::core

#endif // LIBSBX_CORE_LOGGER_HPP_
