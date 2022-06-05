#include "button.hpp"

#include <GLFW/glfw3.h>

namespace demo {

const button button::left{GLFW_MOUSE_BUTTON_LEFT};
const button button::right{GLFW_MOUSE_BUTTON_RIGHT};
const button button::middle{GLFW_MOUSE_BUTTON_MIDDLE};

} // namespace demo
