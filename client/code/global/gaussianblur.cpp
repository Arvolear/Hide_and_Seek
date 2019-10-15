#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"

#include "../window/renderquad.hpp"

#include "gaussianblur.hpp"

template < typename T >
GaussianBlur<T>::GaussianBlur()
{
    blurShader = new Shader();
    blurShader->loadShaders(global.path("code/shader/gaussianBlurShader.vert"), global.path("code/shader/gaussianBlurShader.frag"));
    
    scaleShader = new Shader();
    scaleShader->loadShaders(global.path("code/shader/renderShader.vert"), global.path("code/shader/renderShader.frag"));

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
void GaussianBlur<T>::genBuffer(int width, int height, FrameBufferData data, float scaleFactor)
{
    for (int i = 0; i < 2; i++)
    {
        colorBuffers[i]->genBuffer(width / scaleFactor, height / scaleFactor, {data});
    }

    downscaleBuffer->genBuffer(width / scaleFactor, height / scaleFactor, {data});
    upscaleBuffer->genBuffer(width, height, {data});
    quad->init();
}

template < typename T >
void GaussianBlur<T>::genBuffer(vec2 size, FrameBufferData data, float scaleFactor)
{
    genBuffer(size.x, size.y, data, scaleFactor);
}
    
template < typename T >
GLuint GaussianBlur<T>::blur(GLuint textureID, int intensity, float radius)
{
    glDisable(GL_DEPTH_TEST);

    /* downscale */
    downscaleBuffer->use();

    scaleShader->use();

    glActiveTexture(GL_TEXTURE0);
    scaleShader->setInt("finalTexture", 0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    quad->render(scaleShader);

    bluredTexture = downscaleBuffer->getTexture();

    /* blur */
    int buffer = 0;
    
    for (int i = 0; i < intensity * 2; i++)
    {
        colorBuffers[buffer]->use();
    
        blurShader->use();

        bluredTexture = !i ? bluredTexture : colorBuffers[(buffer + 1) % 2]->getTexture();
        blurShader->setInt("isHorizontal", (buffer + 1) % 2);
        blurShader->setFloat("intensity", radius);
    
        glActiveTexture(GL_TEXTURE0);
        blurShader->setInt("blurTexture", 0);
        glBindTexture(GL_TEXTURE_2D, bluredTexture);
        
        quad->render(blurShader);
    
        buffer = (buffer + 1) % 2;
    }
    
    bluredTexture = colorBuffers[1]->getTexture();

    /* upscale */
    upscaleBuffer->use();
    
    scaleShader->use();

    glActiveTexture(GL_TEXTURE0);
    scaleShader->setInt("finalTexture", 0);
    glBindTexture(GL_TEXTURE_2D, bluredTexture);

    quad->render(scaleShader);
        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
    
    bluredTexture = upscaleBuffer->getTexture();
    return bluredTexture;
}
        
template < typename T >
GLuint GaussianBlur<T>::getTexture() const
{
    return bluredTexture;
}

template < typename T >
pair < GLuint, GLuint > GaussianBlur<T>::getBuffers() const
{
    return {colorBuffers[0]->getBuffer(), colorBuffers[1]->getBuffer()};
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
