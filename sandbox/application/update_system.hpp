#ifndef SBX_APPLICATION_UPDATE_SYSTEM_HPP_
#define SBX_APPLICATION_UPDATE_SYSTEM_HPP_

#include <GLFW/glfw3.h>

#include <core/system.hpp>
#include <core/event_queue.hpp>

namespace sbx {

class update_system : public system<update_system> {

public:

  update_system(event_queue* event_queue, GLFWwindow* handle);
  ~update_system() = default;

  void initialize();
  void update(const time delta_time);
  void finished();
  void aborted();

private:

  event_queue* _event_queue{};
  GLFWwindow* _handle{};

}; // class update_system

} // namespace sbx

#endif // SBX_APPLICATION_UPDATE_SYSTEM_HPP_
