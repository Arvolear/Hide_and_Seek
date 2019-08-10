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
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);

    view = mat4(1.0);
    projection = mat4(1.0);

    shadowBuffer = new DepthColorBuffer();
    gaussianBlur = new GaussianBlur < DepthColorBuffer >();
}

DirLight::DirLight(vec3 dir, vec3 amb, vec3 diff, vec3 spec)
{
    direction = dir;

    ambient = amb;
    diffuse = diff;
    specular = spec;

    shadowBuffer = new DepthColorBuffer();
}

void DirLight::genShadowBuffer(int width, int height)
{
    shadowBuffer->genBuffer(width, height);
    gaussianBlur->genBuffer(width, height);
}

void DirLight::genShadowBuffer(vec2 size)
{
    genShadowBuffer(size.x, size.y);
}

void DirLight::setDirection(vec3 dir)
{
    direction = dir;
}

void DirLight::setAmbient(vec3 amb)
{
    ambient = amb;
}

void DirLight::setDiffuse(vec3 diff)
{
    diffuse = diff;
}

void DirLight::setSpecular(vec3 spec)
{
    specular = spec;
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
    gaussianBlur->blur(shadowBuffer->getTexture(), intensity, radius);
}

void DirLight::render(Shader* shader, GLuint index)
{
    shader->setVec3("dirLights[" + to_string(index) + "].direction", direction);
    shader->setVec3("dirLights[" + to_string(index) + "].ambient", ambient);
    shader->setVec3("dirLights[" + to_string(index) + "].diffuse", diffuse);
    shader->setVec3("dirLights[" + to_string(index) + "].specular", specular);
   
    GLuint shadowTexture;

    if (gaussianBlur->getTexture())
    {
        shadowTexture = gaussianBlur->getTexture();
    }
    else
    {
        shadowTexture = shadowBuffer->getTexture();
    }
    
    glActiveTexture(GL_TEXTURE0 + index + shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    shader->setInt("dirLights[" + to_string(index) + "].texture_shadow1", index + shadowTexture);
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
}
