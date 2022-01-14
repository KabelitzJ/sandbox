#ifndef SBX_UI_CONTEXT_HPP_
#define SBX_UI_CONTEXT_HPP_

#include <types/primitives.hpp>

namespace sbx {

class context {

public:

  context();

  context(const context&) = delete;

  context(context&&) = delete;

  ~context();

  context& operator=(const context&) = delete;

  context& operator=(context&&) = delete;

private:

  void _initialize();

  void _terminate();

  inline static uint32 _use_count{0u};

}; // class context

} // namespace sbx

#endif // SBX_UI_CONTEXT_HPP_
