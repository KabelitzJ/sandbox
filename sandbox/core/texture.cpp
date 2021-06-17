#include "texture.hpp"

#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace sbx {

struct texture_data : public base_resource_data {

  ~texture_data() = default;

  unsigned char* buffer;
  int width;
  int height;
  int channel_count;

};

texture::texture(const std::filesystem::path& path) : _id(0), _active_unit(0) {
  base_resource_data* data = _load(path);
  _initialize(data);
}

texture::~texture() {
  glDeleteTextures(1, &_id);
}

void texture::bind(unsigned int unit) {
  _active_unit = unit;
  glActiveTexture(GL_TEXTURE0 + _active_unit);
  glBindTexture(GL_TEXTURE_2D, _id);
}

void texture::unbind() {
  glActiveTexture(GL_TEXTURE0 + _active_unit);
  glBindTexture(GL_TEXTURE_2D, 0);
  _active_unit = 0;
}

GLuint texture::id() const {
  return _id;
}

texture::texture() : _id(0), _active_unit(0) {
  
}

base_resource_data* texture::_load(const std::filesystem::path& path) {
  texture_data* data = new texture_data();

  data->path = path;

  stbi_set_flip_vertically_on_load(true);

  data->buffer = stbi_load(path.string().c_str(), &data->width, &data->height, &data->channel_count, STBI_rgb_alpha);

  return data;
}

void texture::_initialize(base_resource_data* resource_data) {
  texture_data* data = static_cast<texture_data*>(resource_data);

  glGenTextures(1, &_id);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!data->buffer) {
    std::ostringstream ss;

    ss << "[Error] Could not read texture from file: " << data->path << "!\n";

    throw std::runtime_error(ss.str());
  }

  GLint internalformat = GL_RGBA;
  GLenum format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, internalformat, data->width, data->height, 0, format, GL_UNSIGNED_BYTE, data->buffer);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data->buffer);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  delete data;
}

// void texture::_initialize(const std::filesystem::path& path) {
//   if (_texture_unit_counter == 31) {
//     std::ostringstream ss;

//     ss << "[Error] Max 32 textures are supported!\n";

//     throw std::runtime_error(ss.str());
//   }

//   glGenTextures(1, &_id);

//   _texture_unit = _texture_unit_counter++;

//   glActiveTexture(GL_TEXTURE0 + _texture_unit);

//   glBindTexture(GL_TEXTURE_2D, _id);

//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//   int width;
//   int height;
//   int channel_count;
//   stbi_set_flip_vertically_on_load(true);
//   unsigned char* texture_data = stbi_load(path.string().c_str(), &width, &height, &channel_count, STBI_rgb_alpha);

//   if (!texture_data) {
//     std::ostringstream ss;

//     ss << "[Error] Could not read texture from file: " << path << "!\n";

//     throw std::runtime_error(ss.str());
//   }

//   GLint internalformat = GL_RGBA;
//   GLenum format = GL_RGBA;

//   glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
//   glGenerateMipmap(GL_TEXTURE_2D);

//   stbi_image_free(texture_data);

//   glActiveTexture(GL_TEXTURE0);
//   glBindTexture(GL_TEXTURE_2D, 0);
// }

} // namespace sbx
