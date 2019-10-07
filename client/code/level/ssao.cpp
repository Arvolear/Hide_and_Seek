#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"

#include "../window/renderquad.hpp"

#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "ssao.hpp"

SSAO::SSAO()
{
    colorBuffer = new ColorBuffer();
    gaussBlur = new GaussianBlur < ColorBuffer >();

    radius = bias = 0.0;

    texture_noise = 0;
}

float SSAO::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void SSAO::genBuffer(int width, int height)
{
    this->renderSize.x = width;
    this->renderSize.y = height;

    colorBuffer->genBuffer(width, height, {{GL_RGB16F, GL_RGB, GL_FLOAT}});
}

void SSAO::genBuffer(vec2 size)
{
    genBuffer(size.x, size.y);
}

void SSAO::genSampleKernel(int size)
{
    kernel.clear();

    for (int i = 0; i < size; i++)
    {
        vec3 sphereSample = global.getRandomVec3();
        sphereSample.x = sphereSample.x * 2.0 - 1.0;
        sphereSample.y = sphereSample.y * 2.0 - 1.0;

        sphereSample = normalize(sphereSample);
        sphereSample *= global.getRandomNumber();

        float scale = float(i) / float(size);

        scale = lerp(0.1, 1.0, scale * scale);
        sphereSample *= scale;
        
        kernel.push_back(sphereSample);
    }
}

void SSAO::genNoise(int size)
{
    if (texture_noise)
    {
        glDeleteTextures(1, &texture_noise);
    }

    noise.clear();

    glGenTextures(1, &texture_noise);
    glBindTexture(GL_TEXTURE_2D, texture_noise);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    for (int i = 0; i < size * size; i++)
    {
        vec3 noiseSample = global.getRandomVec3();
        noiseSample.x = noiseSample.x * 2.0 - 1.0;
        noiseSample.y = noiseSample.y * 2.0 - 1.0;
        noiseSample.z = 0.0;

        noise.push_back(noiseSample); 
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, size, size, 0, GL_RED, GL_FLOAT, noise.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::setSoftness(int softness)
{
    gaussBlur->genBuffer(renderSize, {GL_R16F, GL_RGB, GL_FLOAT}, softness); 
}
        
void SSAO::setRadius(float radius)
{
    this->radius = radius;
}

void SSAO::setBias(float bias)
{
    this->bias = bias;
}

void SSAO::blur(int intensity, float radius)
{
    if (colorBuffer->getBuffer())
    {
        gaussBlur->blur(colorBuffer->getTexture(), intensity, radius);
    }
}

void SSAO::renderInfo(Shader* shader)
{
    shader->setInt("kernelSize", kernel.size());
    shader->setInt("noiseSize", sqrt(noise.size()));

    shader->setFloat("radius", radius);
    shader->setFloat("bias", bias);

    glActiveTexture(GL_TEXTURE0 + texture_noise);
    glBindTexture(GL_TEXTURE_2D, texture_noise);
    shader->setInt("texture_noise", texture_noise);

    for (size_t i = 0; i < kernel.size(); i++)
    {
        shader->setVec3("sphereSamples[" + to_string(i) + "]", kernel[i]);    
    }

    shader->setVec2("renderSize", renderSize);
}
        
void SSAO::renderSsao(Shader* shader)
{
    glActiveTexture(GL_TEXTURE0 + getTexture());
    glBindTexture(GL_TEXTURE_2D, getTexture());
    shader->setInt("texture_ssao", getTexture());
}

ColorBuffer* SSAO::getBuffer() const
{
    return colorBuffer;
}

GLuint SSAO::getTexture() const
{
    if (gaussBlur->getBuffers().first && gaussBlur->getTexture())
    {
        return gaussBlur->getTexture();
    }
    else if (colorBuffer->getBuffer())
    {
        return colorBuffer->getTexture();
    }

    return 0;
}

SSAO::~SSAO()
{
    delete colorBuffer;
    delete gaussBlur;

    if (texture_noise)
    {
        glDeleteTextures(1, &texture_noise);
    }
}
