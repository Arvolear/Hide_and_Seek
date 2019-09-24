#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthcolorbuffer.hpp"

#include "../window/renderquad.hpp"

#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "dirlight.hpp"

DirLight::DirLight()
{
    direction = vec3(0.0);
    color = vec3(0.0);

    view = mat4(1.0);
    projection = mat4(1.0);

    shadowBuffer = new DepthColorBuffer();
    gaussianBlur = new GaussianBlur < DepthColorBuffer >();
}

DirLight::DirLight(vec3 direction, vec3 color)
{
    this->direction = direction;
    this->color = color;

    shadowBuffer = new DepthColorBuffer();
    gaussianBlur = new GaussianBlur < DepthColorBuffer >();
}

void DirLight::genShadowBuffer(int width, int height, float blurScale)
{
    shadowBuffer->genBuffer(width, height);
    gaussianBlur->genBuffer(width, height, blurScale);
}

void DirLight::genShadowBuffer(vec2 size, float blurScale)
{
    genShadowBuffer(size.x, size.y, blurScale);
}

void DirLight::setDirection(vec3 dir)
{
    direction = dir;
}

void DirLight::setColor(vec3 color)
{
    this->color = color;
}

vec3 DirLight::getDirection() const
{
    return direction;
}

void DirLight::setView(mat4 view)
{
    this->view = view;
}

void DirLight::setProjection(mat4 projection)
{
    this->projection = projection;
}
        
void DirLight::blur(float intensity, float radius)
{
    if (shadowBuffer->getBuffer())
    {
        gaussianBlur->blur(shadowBuffer->getTexture(), intensity, radius);
    }
}

void DirLight::render(Shader* shader, GLuint index)
{
    shader->setVec3("dirLights[" + to_string(index) + "].direction", direction);
    shader->setVec3("dirLights[" + to_string(index) + "].color", color);

    GLuint shadowTexture = 0;

    if (gaussianBlur->getBuffers().first && gaussianBlur->getTexture())
    {
        shadowTexture = gaussianBlur->getTexture();
    }
    else if (shadowBuffer->getBuffer())
    {
        shadowTexture = shadowBuffer->getTexture();
    }

    if (shadowTexture)
    {
        shader->setInt("dirLights[" + to_string(index) + "].isShadow", 1);
        glActiveTexture(GL_TEXTURE0 + shadowTexture);
        glBindTexture(GL_TEXTURE_2D, shadowTexture);
        shader->setInt("dirLights[" + to_string(index) + "].texture_shadow1", shadowTexture);
    
        shader->setMat4("dirLights[" + to_string(index) + "].shadowView", getView());
        shader->setMat4("dirLights[" + to_string(index) + "].shadowProjection", getProjection());
    }
    else
    {
        shader->setInt("dirLights[" + to_string(index) + "].isShadow", 0);
    }
}

DepthColorBuffer* DirLight::getShadowBuffer() const
{
    return shadowBuffer;
}

void DirLight::updateView(vec3 playerPosition)
{
    view = lookAt(playerPosition, playerPosition + direction, vec3(0, 1, 0));
}

mat4 DirLight::getView() const
{
    return view;
}

mat4 DirLight::getProjection() const
{
    return projection;
}

DirLight::~DirLight()
{
    delete shadowBuffer;
    delete gaussianBlur;
}
