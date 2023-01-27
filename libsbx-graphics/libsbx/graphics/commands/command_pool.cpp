#include <libsbx/graphics/commands/command_pool.hpp>

namespace sbx::graphics {

command_pool::command_pool(const std::thread::id& thread_id)
: _thread_id{thread_id} {

}

command_pool::~command_pool() {

}

} // namespace sbx::graphics
