#ifndef SBX_WINDOW_UPDATE_SYSTEM_HPP_
#define SBX_WINDOW_UPDATE_SYSTEM_HPP_

#include <GLFW/glfw3.h>

#include <core/system.hpp>
#include <core/event_queue.hpp>

namespace sbx {

class update_system final : public system {

public:

  update_system(GLFWwindow* handle);
  ~update_system() = default;

  void initialize() override;
  void update(const time delta_time) override;
  void terminate() override;

private:

  GLFWwindow* _handle{};
  uint32 _frame_counter{};
  time _timer{};

}; // class update_system

} // namespace sbx

#endif // SBX_WINDOW_UPDATE_SYSTEM_HPP_
