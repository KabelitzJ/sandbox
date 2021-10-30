#ifndef SBX_RENDERING_RENDERING_MODULE_HPP_
#define SBX_RENDERING_RENDERING_MODULE_HPP_

#include <core/module.hpp>

namespace sbx {

class rendering_module final : public module {

public:

  rendering_module();
  ~rendering_module() = default;

  void initialize() override;
  void terminate() override;

private:

  void _log_context_info() const;

};

} // namespace sbx

#endif // SBX_RENDERING_RENDERING_MODULE_HPP_
