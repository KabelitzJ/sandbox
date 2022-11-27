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
 * @file libsbx/core/type_name.hpp
 */

#ifndef LIBSBX_CORE_TYPE_NAME_HPP_
#define LIBSBX_CORE_TYPE_NAME_HPP_

/**
 * @ingroup libsbx-core
 */

#include <cstdlib>
#include <string>
#include <typeindex>

#include <cxxabi.h>

#include <libsbx/core/platform.hpp>

namespace sbx::core {

std::string type_name(const std::type_index& type) {
#if defined(LIBSBX_COMPILER_GNU)
  auto status = 0;
  auto name = std::string{type.name()};
  auto* demangled_name = abi::__cxa_demangle(name.c_str(), NULL, NULL, &status);

  if(status == 0) {
    name = demangled_name;
    std::free(demangled_name);
  }

  return name;
#else
  return type.name();
#endif
}

} // namespace sbx::core

#endif // LIBSBX_CORE_TYPE_NAME_HPP_
