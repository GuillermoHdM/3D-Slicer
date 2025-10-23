#ifndef OPENGL_HPP
#define OPENGL_HPP

#include <glad/glad.h>  // OpenGl library
#include <GLFW/glfw3.h> // For contexts
#include <stdexcept>
#include "Math.hpp"

struct Triangle
{
	glm::vec3 n;
	glm::vec3 A;
	glm::vec3 B;
	glm::vec3 C;
};
GLuint CreateProgram(GLenum stage, const char const* ShaderText);
#endif // OPENGL_HPP
