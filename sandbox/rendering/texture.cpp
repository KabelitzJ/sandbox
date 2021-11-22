#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/logger.hpp>

namespace sbx {

texture::texture(const std::string& path)
: _texture_id{0} {
  _load(path);
}

texture::~texture() {
  glDeleteTextures(1, &_texture_id);
}

void texture::bind(const gl_texture_unit texture_unit) const {
  glActiveTexture(GL_TEXTURE0 + texture_unit);
  glBindTexture(GL_TEXTURE_2D, _texture_id);
} 

void texture::_load(const std::string& path) {
  stbi_set_flip_vertically_on_load(true);

  glGenTextures(1, &_texture_id);
  glBindTexture(GL_TEXTURE_2D, _texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  auto width = 0;
  auto height = 0;
  auto channel_count = 0;

  auto data = stbi_load(path.c_str(), &width, &height, &channel_count, 0);

  if (data) {
    if (channel_count == 4) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (channel_count == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
      logger::warn("texture::_load: unsupported channel count: {}", channel_count);
    }

    logger::debug("Loaded texture: {} with width {} height {} and channels {}", path, width, height, channel_count);

    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    logger::error("Failed to load texture: {}", path);
  }

  stbi_image_free(data);
}

} // namespace sbx