#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthcolorbuffer.hpp"

#include "../window/renderquad.hpp"

#include "../global/radialblur.hpp"
#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "../game_object/sphere.hpp"

#include "dirlight.hpp"

DirLight::DirLight()
{
    direction = color = vec3(0.0);

    shadowView = shadowProjection = mat4(1.0);

    sphere = new Sphere();

    shadowBuffer = new DepthColorBuffer();
    gaussianBlur = new GaussianBlur < DepthColorBuffer >();

    scatterBuffer = new ColorBuffer();
    radialBlur = new RadialBlur();
}

DirLight::DirLight(vec3 direction, vec3 color)
{
    this->direction = direction;
    this->color = color;

    sphere = new Sphere();

    shadowBuffer = new DepthColorBuffer();
    gaussianBlur = new GaussianBlur < DepthColorBuffer >();
    
    scatterBuffer = new ColorBuffer();
    radialBlur = new RadialBlur();
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

void DirLight::genScatterBuffer(int width, int height, float blurScale)
{
    scatterBuffer->genBuffer(width, height);
    radialBlur->genBuffer(width, height, blurScale);
}

void DirLight::genScatterBuffer(vec2 size, float blurScale)
{
    genScatterBuffer(size.x, size.y, blurScale);
}
        
void DirLight::genSphere(vec3 center, double radius, int depth)
{
    sphere->construct(center, radius, depth);
}

void DirLight::setDirection(vec3 dir)
{
    direction = dir;
}

void DirLight::setColor(vec3 color)
{
    this->color = color;
}

void DirLight::setSphereColor(vec3 color)
{
    sphere->setColor(color);
}

void DirLight::setExposure(float exposure)
{
    radialBlur->setExposure(exposure);
}

void DirLight::setDecay(float decay)
{
    radialBlur->setDecay(decay);
}

void DirLight::setDensity(float density)
{
    radialBlur->setDensity(density);
}

void DirLight::setWeight(float weight)
{
    radialBlur->setWeight(weight);
} 

vec3 DirLight::getDirection() const
{
    return direction;
}

void DirLight::setShadowProjection(mat4 projection)
{
    this->shadowProjection = projection;
}
        
void DirLight::blurShadow(float intensity, float radius)
{
    if (shadowBuffer->getBuffer())
    {
        gaussianBlur->blur(shadowBuffer->getTexture(), intensity, radius);
    }
}

void DirLight::blurScatter(vec2 center)
{
    if (scatterBuffer->getBuffer())
    {
        center.x = (center.x + 1.0) / 2.0;
        center.y = (center.y + 1.0) / 2.0;

        radialBlur->blur(scatterBuffer->getTexture(), center);
    }
}

void DirLight::renderShadow(Shader* shader, GLuint index)
{
    shader->setVec3("dirLights[" + to_string(index) + "].direction", direction);
    shader->setVec3("dirLights[" + to_string(index) + "].color", color);

    GLuint shadowTexture = getShadowTexture();

    if (shadowTexture)
    {
        shader->setInt("dirLights[" + to_string(index) + "].isShadow", 1);

        glActiveTexture(GL_TEXTURE0 + shadowTexture);
        glBindTexture(GL_TEXTURE_2D, shadowTexture);
        shader->setInt("dirLights[" + to_string(index) + "].texture_shadow1", shadowTexture);
    
        shader->setMat4("dirLights[" + to_string(index) + "].shadowView", getShadowView());
        shader->setMat4("dirLights[" + to_string(index) + "].shadowProjection", getShadowProjection());
    }
    else
    {
        shader->setInt("dirLights[" + to_string(index) + "].isShadow", 0);
    }
}
        
void DirLight::renderSphere(Shader* shader)
{
    glDepthFunc(GL_LEQUAL);

    sphere->render(shader); 
    
    glDepthFunc(GL_LESS);
}
        
void DirLight::renderLight(Shader* shader)
{
    shader->setInt("scatterTexture", getScatterTexture());
    glActiveTexture(GL_TEXTURE0 + getScatterTexture());
    glBindTexture(GL_TEXTURE_2D, getScatterTexture());
}

GLuint DirLight::getShadowTexture() const
{
    if (gaussianBlur->getBuffers().first && gaussianBlur->getTexture())
    {
        return gaussianBlur->getTexture();
    }
    else if (shadowBuffer->getBuffer())
    {
        return shadowBuffer->getTexture();
    }

    return 0;
}

GLuint DirLight::getScatterTexture() const
{
    if (radialBlur->getBuffer() && radialBlur->getTexture())
    {
        return radialBlur->getTexture();
    }
    else if (scatterBuffer->getBuffer())
    {
        return scatterBuffer->getTexture();
    }

    return 0;
}

DepthColorBuffer* DirLight::getShadowBuffer() const
{
    return shadowBuffer;
}

ColorBuffer* DirLight::getScatterBuffer() const
{
    return scatterBuffer;
}

void DirLight::updateShadowView(vec3 playerPosition)
{
    shadowView = lookAt(playerPosition, playerPosition + direction, vec3(0, 1, 0));
}

Sphere* DirLight::getSphere() const
{
    return sphere;
}

mat4 DirLight::getShadowView() const
{
    return shadowView;
}

mat4 DirLight::getShadowProjection() const
{
    return shadowProjection;
}

DirLight::~DirLight()
{
    delete shadowBuffer;
    delete gaussianBlur;

    delete scatterBuffer;
    delete radialBlur;
}
