#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "gaussianblur.hpp"

GaussianBlur::GaussianBlur(Window* window)
{
    blurShader = new Shader();
    blurShader->loadShaders(path("code/shader/vertexGaussBlurShader.glsl"), path("code/shader/fragmentGaussBlurShader.glsl"));

    for (int i = 0; i < 2; i++)
    {
        colorBuffers.push_back(new ColorBuffer());
        colorBuffers[i]->genBuffer(window->getRenderSize() / vec2(2.0));
    }
    
    quad = new RenderQuad();
    quad->init();
}

    
GLuint GaussianBlur::blur(GLuint textureID, float intensity)
{
    int buffer = 0;
    
    for (int i = 0; i < intensity; i++)
    {
        colorBuffers[buffer]->use();
        colorBuffers[buffer]->clear();
        
        blurShader->use();

        textureID = !i ? textureID : colorBuffers[(buffer + 1) % 2]->getTexture();
        blurShader->setInt("isHorizontal", (buffer + 1) % 2);
        blurShader->setFloat("intensity", intensity);
    
        blurShader->setInt("blurTexture", textureID);
        glActiveTexture(GL_TEXTURE0 + textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        quad->render(blurShader);
    
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        buffer = (buffer + 1) % 2;
    }
    
    return colorBuffers[1]->getTexture();
}

GaussianBlur::~GaussianBlur()
{
    delete blurShader;

    for (size_t i = 0; i < colorBuffers.size(); i++)
    {
        delete colorBuffers[i];
    }

    delete quad;
}
