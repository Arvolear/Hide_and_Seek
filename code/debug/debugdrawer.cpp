#include "../shader/shader.hpp"

#include "debugdrawer.hpp"

DebugDrawer::DebugDrawer() : btIDebugDraw() 
{
    VAO = VBO = 0;
    debugMode = 0;
}

void DebugDrawer::setDebugMode(int debugMode)
{
    this->debugMode = debugMode;
}

int DebugDrawer::getDebugMode() const
{
    return debugMode;
}

void DebugDrawer::init()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Lpos) + sizeof(Lcolor), 0, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Lpos), (void*)(Lpos));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Lpos), sizeof(Lcolor), (void*)(Lcolor));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); //vertex positions 
    glEnableVertexAttribArray(0); //enable vertex positions
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Lpos))); //color 
    glEnableVertexAttribArray(1); //enable vertex color
     
    glBindVertexArray(0);
}

void DebugDrawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{
    const btVector3 startPoint = pointOnB;
    const btVector3 endPoint = pointOnB + normalOnB * distance;

    drawLine(startPoint, endPoint, color);
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    Lpos[0] = from.x();
    Lpos[1] = from.y();
    Lpos[2] = from.z();

    Lpos[3] = to.x();
    Lpos[4] = to.y();
    Lpos[5] = to.z();

    Lcolor[0] = color.x();
    Lcolor[1] = color.y();
    Lcolor[2] = color.z();
    
    Lcolor[3] = color.x();
    Lcolor[4] = color.y();
    Lcolor[5] = color.z();

    init();

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
        
void DebugDrawer::reportErrorWarning(const char *warningString){}

void DebugDrawer::draw3dText(const btVector3 &location, const char* textString){}

void DebugDrawer::applyViewProjection(Shader* shader, mat4 view, mat4 projection)
{
    this->shader = shader;
    this->view = view;
    this->projection = projection;
}

void DebugDrawer::updateViewProjection()
{
    shader->setMat4("transform", mat4(1.0));
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
}

DebugDrawer::~DebugDrawer() 
{
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
    }
    
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
    }
}
