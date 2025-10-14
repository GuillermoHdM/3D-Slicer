#ifndef OPENGL_HPP
#define OPENGL_HPP

#include <glad/glad.h>  // OpenGl library
#include <GLFW/glfw3.h> // For contexts
#include <stdexcept>


GLuint CreateProgram(GLenum stage, const char const* ShaderText);
#endif // OPENGL_HPP
