#ifndef LIBSBX_SCRIPTING_FUNCTION_HPP_
#define LIBSBX_SCRIPTING_FUNCTION_HPP_

#include <string>

namespace sbx::scripting {

class function {

public:

  template<typename Return, typename... Args>
  Return call(Args&&... args) {
    return Return{};
  }

private:

}; // class function

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_FUNCTION_HPP_
