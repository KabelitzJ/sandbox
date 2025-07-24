#include <libsbx/graphics/render_graph.hpp>

namespace sbx::graphics {

namespace detail {

auto graph_base::_reserve(const std::size_t capacity) -> void {
  base::reserve(capacity);
}

auto graph_base::_erase(graph_node* node) -> void {
  base::erase(std::remove_if(base::begin(), base::end(), [&](auto& entry){ return entry.get() == node; }), base::end() );
} 

graph_node::graph_node()
: _data{nullptr},
  _parent{nullptr},
  _num_successors{0u} { }

auto graph_node::num_successors() const -> std::size_t {
  return _num_successors;
}

auto graph_node::num_predecessors() const -> std::size_t {
  return _edges.size() - _num_successors;
}

auto graph_node::name() const -> const std::string& {
  return _name;
}

auto graph_node::attachments() const -> const std::vector<attachment>& {
  return _outputs;
}

auto graph_node::_precede(graph_node* node) -> void {
  _edges.push_back(node);
  std::swap(_edges[_num_successors++], _edges[_edges.size() - 1]);
  node->_edges.push_back(this);
}

auto graph_node::_remove_successors(graph_node* node) -> void {
  auto sit = std::remove(_edges.begin(), _edges.begin() + _num_successors, node);
  size_t new_num_successors = std::distance(_edges.begin(), sit);
  std::move(_edges.begin() + _num_successors, _edges.end(), sit);
  _edges.resize(_edges.size() - (_num_successors - new_num_successors));
  _num_successors = new_num_successors;
}

auto graph_node::_remove_predecessors(graph_node* node) -> void {
  _edges.erase(std::remove(_edges.begin() + _num_successors, _edges.end(), node), _edges.end());
}

// auto pass::name() const -> const std::string& {
//   return _node->name();
// }

// auto pass::num_predecessors() const -> std::size_t {
//   return _node->num_predecessors();
// }

// auto pass::num_successors() const -> std::size_t {
//   return _node->num_predecessors();
// }

// pass::pass(graph_node* node)
// : _node{node} { }

graph_builder::graph_builder(graph_base& graph)
: _graph{graph} { }

} // namespace detail

render_graph::render_graph(const std::string& name)
: base{_graph},
  _name{name} { }

} // namespace sbx::graphics
