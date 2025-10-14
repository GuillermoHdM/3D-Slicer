#include "OpenGl.hpp"
#include <stdexcept>

GLuint CreateProgram(GLenum stage, const char const* ShaderText)
{
    GLuint Shader = glCreateShader(stage);
    if (stage == GL_VERTEX_SHADER)
    {
        glShaderSource(Shader, 1, &ShaderText, nullptr);
    }
    else if (stage == GL_FRAGMENT_SHADER)
    {
        glShaderSource(Shader, 1, &ShaderText, nullptr);
    }
    else
    {
        throw std::runtime_error("ERROR: Trying to create a shader I dont support");
    }
    glCompileShader(Shader);

    // Check for errors
    int IsOk = 0;
    char infoLog[512];
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsOk);
    if (!IsOk) {
        glGetShaderInfoLog(Shader, 512, nullptr, infoLog);
        throw std::runtime_error("ERROR: there was an error in the shader");
    }

    return Shader;

}
