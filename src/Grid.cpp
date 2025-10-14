#include "Grid.h"
#include "Shaders.h"
void Grid::Init()
{
    //To draw the grid on
    float GridQuad[] =
    {
        -1000.0f, 0.0f, -1000.0f,
         1000.0f, 0.0f, -1000.0f,
        -1000.0f, 0.0f,  1000.0f,
         1000.0f, 0.0f,  1000.0f
    };
    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);
    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GridQuad), GridQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    m_VtxShader = CreateProgram(GL_VERTEX_SHADER, GridVtxShader);
    m_FrgShader = CreateProgram(GL_FRAGMENT_SHADER, GridFragShader);
    m_Shader = glCreateProgram();
    glAttachShader(m_Shader, m_VtxShader);
    glAttachShader(m_Shader, m_FrgShader);
    glLinkProgram(m_Shader);
}
void Grid::Clear()
{
    glDeleteVertexArrays(1, &m_Vao);
    glDeleteBuffers(1, &m_Vbo);
    glDeleteProgram(m_Shader);
}
void Grid::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    glUseProgram(m_Shader);
    glUniformMatrix4fv(glGetUniformLocation(m_Shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_Shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glBindVertexArray(m_Vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
