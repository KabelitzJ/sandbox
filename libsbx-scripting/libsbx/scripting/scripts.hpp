#ifndef LIBSBX_SCRIPTING_SCRIPTS_HPP_
#define LIBSBX_SCRIPTING_SCRIPTS_HPP_

#include <cstdint>

#include <string>

#include <libsbx/scenes/node.hpp>

namespace sbx::scripting {

class scripts {

public:

  scripts(const scenes::node node)
  : _node{node} { }

  auto add(const std::string& script) -> void;

private:

  scenes::node _node;

}; // class scripts

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_SCRIPTS_HPP_