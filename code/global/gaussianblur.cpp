#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"

#include "gaussianblur.hpp"

template < typename T >
GaussianBlur<T>::GaussianBlur()
{
    blurShader = new Shader();
    blurShader->loadShaders(path("code/shader/gaussianBlurShader.vert"), path("code/shader/gaussianBlurShader.frag"));
    
    scaleShader = new Shader();
    scaleShader->loadShaders(path("code/shader/renderShader.vert"), path("code/shader/renderShader.frag"));

    for (int i = 0; i < 2; i++)
    {
        colorBuffers.push_back(new T());
    }
        
    upscaleBuffer = new T();
    downscaleBuffer = new T();
    
    quad = new RenderQuad();

    bluredTexture = 0;
}

template < typename T >
void GaussianBlur<T>::genBuffer(float width, float height, float scaleFactor)
{
    for (int i = 0; i < 2; i++)
    {
        colorBuffers[i]->genBuffer(width / scaleFactor, height / scaleFactor);
    }

    downscaleBuffer->genBuffer(width / scaleFactor, height / scaleFactor);
    upscaleBuffer->genBuffer(width, height);
    quad->init();
}

template < typename T >
void GaussianBlur<T>::genBuffer(vec2 size, float scaleFactor)
{
    genBuffer(size.x, size.y, scaleFactor);
}
    
template < typename T >
GLuint GaussianBlur<T>::blur(GLuint textureID, float intensity, float radius)
{
    /* downscale */
    downscaleBuffer->use();
    downscaleBuffer->clear();

    scaleShader->use();

    scaleShader->setInt("finalTexture", textureID);
    glActiveTexture(GL_TEXTURE0 + textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    quad->render(scaleShader);

    bluredTexture = downscaleBuffer->getTexture();

    /* blur */
    int buffer = 0;
    
    for (int i = 0; i < intensity; i++)
    {
        colorBuffers[buffer]->use();
        colorBuffers[buffer]->clear();
        
        blurShader->use();

        bluredTexture = !i ? bluredTexture : colorBuffers[(buffer + 1) % 2]->getTexture();
        blurShader->setInt("isHorizontal", (buffer + 1) % 2);
        blurShader->setFloat("intensity", radius);
    
        blurShader->setInt("blurTexture", bluredTexture);
        glActiveTexture(GL_TEXTURE0 + bluredTexture);
        glBindTexture(GL_TEXTURE_2D, bluredTexture);
        
        quad->render(blurShader);
    
        buffer = (buffer + 1) % 2;
    }
    
    bluredTexture = colorBuffers[1]->getTexture();

    /* upscale */
    upscaleBuffer->use();
    upscaleBuffer->clear();

    scaleShader->use();

    scaleShader->setInt("finalTexture", bluredTexture);
    glActiveTexture(GL_TEXTURE0 + bluredTexture);
    glBindTexture(GL_TEXTURE_2D, bluredTexture);

    quad->render(scaleShader);
        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    bluredTexture = upscaleBuffer->getTexture();
    return bluredTexture;
}
        
template < typename T >
GLuint GaussianBlur<T>::getTexture() const
{
    return bluredTexture;
}

template < typename T >
GaussianBlur<T>::~GaussianBlur()
{
    delete blurShader;
    delete scaleShader;

    for (size_t i = 0; i < colorBuffers.size(); i++)
    {
        delete colorBuffers[i];
    }

    delete upscaleBuffer;
    delete downscaleBuffer;

    delete quad;
}
