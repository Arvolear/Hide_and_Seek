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

    for (int i = 0; i < 2; i++)
    {
        colorBuffers.push_back(new T());
    }
    
    quad = new RenderQuad();
}

template < typename T >
void GaussianBlur<T>::genBuffer(float width, float height)
{
    for (int i = 0; i < 2; i++)
    {
        colorBuffers[i]->genBuffer(width, height);
    }

    quad->init();
}

template < typename T >
void GaussianBlur<T>::genBuffer(vec2 size)
{
    genBuffer(size.x, size.y);
}
    
template < typename T >
GLuint GaussianBlur<T>::blur(GLuint textureID, float intensity, float radius)
{
    int buffer = 0;
    
    for (int i = 0; i < intensity; i++)
    {
        colorBuffers[buffer]->use();
        colorBuffers[buffer]->clear();
        
        blurShader->use();

        textureID = !i ? textureID : colorBuffers[(buffer + 1) % 2]->getTexture();
        blurShader->setInt("isHorizontal", (buffer + 1) % 2);
        blurShader->setFloat("intensity", radius);
    
        blurShader->setInt("blurTexture", textureID);
        glActiveTexture(GL_TEXTURE0 + textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        quad->render(blurShader);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        buffer = (buffer + 1) % 2;
    }
    
    bluredTexture = colorBuffers[1]->getTexture();
    return colorBuffers[1]->getTexture();
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

    for (size_t i = 0; i < colorBuffers.size(); i++)
    {
        delete colorBuffers[i];
    }

    delete quad;
}
