#ifndef TASK_GRAPH_BASIC_TASK_GRAPH_HPP_
#define TASK_GRAPH_BASIC_TASK_GRAPH_HPP_

#include <thread_pool.hpp>

namespace tg {

template<typename Function>
class basic_task_graph {

public:

private:
  tp::thread_pool _pool;

}; // class basic_task_graph

} // namespace tg

#endif // TASK_GRAPH_BASIC_TASK_GRAPH_HPP_