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

namespace sbx::core {

class logger {

public:

  logger() = delete;

  ~logger() = default;

  template<typename... Args>
  static void trace(spdlog::format_string_t<Args...> format, Args&&... args) {
    _logger().trace(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static void trace(const Type& value) {
    _logger().trace(value);
  }

  template<typename... Args>
  static void debug(spdlog::format_string_t<Args...> format, Args&&... args) {
    _logger().debug(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static void debug(const Type& value) {
    _logger().debug(value);
  }

  template<typename... Args>
  static void info(spdlog::format_string_t<Args...> format, Args&&... args) {
    _logger().info(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static void info(const Type& value) {
    _logger().info(value);
  }

  template<typename... Args>
  static void warn(spdlog::format_string_t<Args...> format, Args&&... args) {
    _logger().warn(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static void warn(const Type& value) {
    _logger().warn(value);
  }

  template<typename... Args>
  static void error(spdlog::format_string_t<Args...> format, Args&&... args) {
    _logger().error(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static void error(const Type& value) {
    _logger().error(value);
  }

  template<typename... Args>
  static void critical(spdlog::format_string_t<Args...> format, Args&&... args) {
    _logger().critical(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static void critical(const Type& value) {
    _logger().critical(value);
  }

private:

  static spdlog::logger& _logger() {
    static auto logger = spdlog::logger{"sandbox", std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};
    logger.set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] : %v");
    logger.set_level(spdlog::level::trace);

    return logger;
  }

}; // class logger

} // namespace sbx::core

#endif // LIBSBX_CORE_LOGGER_HPP_
