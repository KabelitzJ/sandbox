#ifndef SBX_TYPES_GL_HPP_
#define SBX_TYPES_GL_HPP_

#include <glad/glad.h>

namespace sbx {

using gl_buffer = GLuint;
using gl_texture = GLuint;
using gl_shader = GLuint;
using gl_program = GLuint;
using gl_framebuffer = GLuint;
using gl_uniform_location = GLint;

using gl_size_ptr = GLsizeiptr;
using gl_size = GLsizei;

} // namespace sbx

#endif // SBX_TYPES_GL_HPP_
