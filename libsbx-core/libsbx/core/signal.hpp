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
 * @file libsbx/core/signal.hpp
 */

#ifndef LIBSBX_CORE_SIGNAL_HPP_
#define LIBSBX_CORE_SIGNAL_HPP_

/**
 * @ingroup libsbx-core
 */

#include <memory>
#include <vector>

#include <libsbx/core/slot.hpp>

namespace sbx::core {

template<typename... Args>
class signal {

public:

  signal() = default;

  void connect(const slot<Args...>& slot) {
    _slots.push_back(&slot);    
  }

  void disconnect(const slot<Args...>& slot) {
    std::erase_if(_slots, [&slot](const auto* entry){ return entry == &slot; });
  }

  void emit(Args... args) {
    for (auto& slot : _slots) {
      std::invoke(*slot, std::forward<Args>(args)...);
    }
  }

  bool is_empty() const noexcept {
    return _slots.empty();  
  }

  void clear() {
    _slots.clear();
  }

private:

  std::vector<const slot<Args...>*> _slots{};

}; // class signal

} // namespace sbx::core

#endif // LIBSBX_CORE_SIGNAL_HPP_
