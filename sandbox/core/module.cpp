#include "module.hpp"

namespace sbx {

registry* module::_registry{nullptr};
scheduler* module::_scheduler{nullptr};
event_queue* module::_event_queue{nullptr};

} // namespace sbx
