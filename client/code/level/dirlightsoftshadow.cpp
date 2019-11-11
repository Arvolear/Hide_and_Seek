#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"

#include "../window/renderquad.hpp"

#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "dirlightsoftshadow.hpp"

DirLightSoftShadow::DirLightSoftShadow()
{
    shadowBuffer = new ShadowBuffer();
    gaussBlur = new GaussianBlur < ShadowBuffer >();

    width = height = 0;
    intensity = bias = 0.0;
    softness = 0;
}

void DirLightSoftShadow::genBuffer(int width, int height)
{
    this->width = width;
    this->height = height;

    shadowBuffer->genBuffer(width, height, 
            {
                {GL_R32F, GL_RED, GL_FLOAT}
            });
}

void DirLightSoftShadow::genBuffer(vec2 size)
{
    genBuffer(size.x, size.y);
}
        
void DirLightSoftShadow::setIntensity(float intensity)
{
    this->intensity = intensity;
}

void DirLightSoftShadow::setBias(float bias)
{
    this->bias = bias;
}

void DirLightSoftShadow::setSoftness(float softness)
{
    this->softness = softness;

    gaussBlur->genBuffer(width, height, {GL_R32F, GL_RED, GL_FLOAT}, softness);
}

void DirLightSoftShadow::blurShadow(int intensity, float radius)
{
    if (shadowBuffer->getBuffer())
    {
        gaussBlur->blur(shadowBuffer->getTexture(), intensity, radius);
    }
}

void DirLightSoftShadow::render(Shader* shader, int index)
{
    glActiveTexture(GL_TEXTURE0 + 5);
    shader->setInt("dirLights[" + to_string(index) + "].texture_shadow1", 5);
    glBindTexture(GL_TEXTURE_2D, getTexture());

    shader->setFloat("dirLights[" + to_string(index) + "].esmFactor", intensity);
    shader->setFloat("dirLights[" + to_string(index) + "].bias", bias);
}

ShadowBuffer* DirLightSoftShadow::getBuffer() const
{
    return shadowBuffer;
}

GLuint DirLightSoftShadow::getTexture() const
{
    if (gaussBlur->getBuffers().first && gaussBlur->getTexture())
    {
        return gaussBlur->getTexture();
    }
    else if (shadowBuffer->getBuffer())
    {
        return shadowBuffer->getTexture();
    }

    return 0;
}

DirLightSoftShadow::~DirLightSoftShadow()
{
    delete shadowBuffer;
    delete gaussBlur;
}
