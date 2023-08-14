#include <libsbx/core/engine.hpp>

namespace sbx::core {

engine* engine::_instance{nullptr};

units::second engine::_delta_time{};

} // namespace sbx::core
