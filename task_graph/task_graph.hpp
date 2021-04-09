#ifndef TASK_GRAPH_HPP_
#define TASK_GRAPH_HPP_

#include <functional>

#include "basic_task_graph.hpp"

namespace tg {

using task_graph = basic_task_graph<std::function<void()>>;

} // namespace tg

#endif // TASK_GRAPH_HPP_