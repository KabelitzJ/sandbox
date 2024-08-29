#ifndef LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
#define LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_

#include <vector>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::graphics {

class pass {

}; // class pass

class graph {

public:

  class node {

  }; // class node

private:

  std::vector<node> _nodes;

}; // class graph

class graph_builder {

public:

protected:

  graph_builder(graph& graph)
  : _graph{memory::make_observer(graph)} { }

private:

  memory::observer_ptr<graph> _graph;

}; // class graph_builder

class render_graph : public graph_builder {

  using base = graph_builder;

public:

  render_graph()
  : base{_graph} { }

private:

  graph _graph;

}; // class render_graph

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
