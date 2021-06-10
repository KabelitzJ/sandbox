#ifndef SBX_CORE_VERTEX_ATTRIBUTE_LAYOUT_HPP_
#define SBX_CORE_VERTEX_ATTRIBUTE_LAYOUT_HPP_

#include <vector>

#include <glad/glad.h>

namespace sbx {

struct vertex_attribute {
  GLuint index;
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  const void* pointer;
}; // struct vertex_attribute

class vertex_attribute_layout {

public:
  vertex_attribute_layout(const std::initializer_list<vertex_attribute>& attributes);
  ~vertex_attribute_layout() = default;

  std::vector<vertex_attribute>::const_iterator begin() const;
  std::vector<vertex_attribute>::const_iterator end() const;

private:
  std::vector<vertex_attribute> _attributes;

}; // class vertex_attribute_layout

} // namespace sbx

#endif // SBX_CORE_VERTEX_ATTRIBUTE_LAYOUT_HPP_
