#ifndef SBX_RENDERING_SHADER_HPP_
#define SBX_RENDERING_SHADER_HPP_

#include <string>
#include <unordered_map>

#include <types/primitives.hpp>

namespace sbx {

class shader {

public:
  explicit shader(const std::string& vertex_path, const std::string& fragment_path);
  ~shader();

  void bind() const;
  void unbind() const;

  void set_int32(const std::string& name, int value);
  void set_float32(const std::string& name, float value);

private:

  void _load(const std::string& vertex_path, const std::string& fragment_path);

  int32 _get_uniform_location(const std::string& name);

  uint32 _id{};
  std::unordered_map<std::string, int32> _uniform_locations{};

};

} // namespace sbx

#endif // SBX_RENDERING_SHADER_HPP_
