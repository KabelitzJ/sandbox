#ifndef SBX_APPLICATION_INPUT_SYSTEM_HPP_
#define SBX_APPLICATION_INPUT_SYSTEM_HPP_

#include <GLFW/glfw3.h>

#include <core/system.hpp>
#include <core/event_queue.hpp>

namespace sbx {

class input_system final : public system {

public:

  input_system(event_queue* event_queue, GLFWwindow* handle);
  ~input_system() = default;

  void initialize() override;
  void update(const time delta_time) override;
  void finished() override;
  void aborted() override;

private:

  event_queue* _event_queue{};
  GLFWwindow* _handle{};

}; // class input_system

} // namespace sbx

#endif // SBX_APPLICATION_INPUT_SYSTEM_HPP_
