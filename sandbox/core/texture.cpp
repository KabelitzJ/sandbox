#include "texture.hpp"

#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace sbx {

unsigned int texture::_texture_unit_counter = 0;

texture::texture(const std::filesystem::path& path) : _id(0), _texture_unit(0) {
  _initialize(path);
}

texture::~texture() {
  glDeleteTextures(1, &_id);
}

void texture::bind() const {
  glActiveTexture(GL_TEXTURE0 + _texture_unit);
  glBindTexture(GL_TEXTURE_2D, _id);
}

void texture::unbind() const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint texture::id() const {
  return _id;
}

unsigned int texture::unit() const {
  return _texture_unit;
}

void texture::_initialize(const std::filesystem::path& path) {
  if (_texture_unit_counter == 31) {
    std::ostringstream ss;

    ss << "[Error] Max 32 textures are supported!\n";

    throw std::runtime_error(ss.str());
  }

  glGenTextures(1, &_id);

  _texture_unit = _texture_unit_counter++;

  glActiveTexture(GL_TEXTURE0 + _texture_unit);

  glBindTexture(GL_TEXTURE_2D, _id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width;
  int height;
  int channel_count;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* texture_data = stbi_load(path.string().c_str(), &width, &height, &channel_count, 0);

  if (!texture_data) {
    std::ostringstream ss;

    ss << "[Error] Could not read texture from file: " << path << "!\n";

    throw std::runtime_error(ss.str());
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(texture_data);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace sbx
