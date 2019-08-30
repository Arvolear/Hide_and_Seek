#include "../shader/shader.hpp"

#include "renderquad.hpp"

RenderQuad::RenderQuad()
{
    VAO = 0;
    VBO = 0;
}

void RenderQuad::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void RenderQuad::render(Shader *shader) const
{
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

RenderQuad::~RenderQuad() 
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
