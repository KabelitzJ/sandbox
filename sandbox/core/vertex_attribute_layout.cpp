#include "vertex_attribute_layout.hpp"

namespace sbx {

vertex_attribute_layout::vertex_attribute_layout(const std::initializer_list<vertex_attribute>& attributes)
: _attributes(attributes) {

}

std::vector<vertex_attribute>::const_iterator vertex_attribute_layout::begin() const {
  return _attributes.begin();
}

std::vector<vertex_attribute>::const_iterator vertex_attribute_layout::end() const {
  return _attributes.end();
}

} // namespace sbx
