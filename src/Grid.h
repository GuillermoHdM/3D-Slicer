#pragma once
#include "Math.hpp"
#include "OpenGl.hpp"
class Grid
{
	GLuint m_Vao;
	GLuint m_Vbo;
	GLuint m_VtxShader;
	GLuint m_FrgShader;
	GLuint m_Shader;
public:
	void Init();
	void Clear();
	void Draw(const glm::mat4& view, const glm::mat4& projection);
};