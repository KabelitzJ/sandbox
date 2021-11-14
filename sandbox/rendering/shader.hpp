#ifndef SBX_RENDERING_SHADER_HPP_
#define SBX_RENDERING_SHADER_HPP_

#include <string>
#include <unordered_map>

#include <types/primitives.hpp>
#include <types/vector.hpp>
#include <types/matrix.hpp>
#include <types/gl.hpp>

namespace sbx {

class shader {

public:
  explicit shader(const std::string& vertex_path, const std::string& fragment_path);
  ~shader();

  void bind() const;
  void unbind() const;

  void set_int32(const std::string& name, int32 value);
  void set_float32(const std::string& name, float32 value);

  void set_vector2(const std::string& name, const vector2& value);
  void set_vector3(const std::string& name, const vector3& value);
  void set_vector4(const std::string& name, const vector4& value);

  void set_matrix3x3(const std::string& name, const matrix3x3& value);
  void set_matrix4x4(const std::string& name, const matrix4x4& value);

private:

  void _load(const std::string& vertex_path, const std::string& fragment_path);

  gl_uniform_location _get_uniform_location(const std::string& name);

  gl_program _program_id{};
  std::unordered_map<std::string, gl_uniform_location> _uniform_locations{};

};

} // namespace sbx

#endif // SBX_RENDERING_SHADER_HPP_
