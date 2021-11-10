#ifndef SBX_RENDERING_RENDER_SYSTEM_HPP_
#define SBX_RENDERING_RENDER_SYSTEM_HPP_

#include <core/system.hpp>
#include <core/event_queue.hpp>

namespace sbx {

class render_system final : public system {

public:

  render_system() = default;
  ~render_system() = default;

  void initialize() override;
  void update(time delta_time) override;
  void terminate() override;

private:

}; // class render_system

} // namespace sbx

#endif // SBX_RENDERING_RENDER_SYSTEM_HPP_
