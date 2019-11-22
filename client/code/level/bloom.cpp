#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "bloom.hpp"

Bloom::Bloom()
{
    bloomTexture = 0;
    
    gaussianBlur = new GaussianBlur < ColorBuffer >();
}

void Bloom::setBloomTexture(GLuint bloomTexture)
{
    this->bloomTexture = bloomTexture;
}

void Bloom::genBlurBuffer(int width, int height, float blurScale)
{
    gaussianBlur->genBuffer(width, height, {GL_RGBA16F, GL_RGBA, GL_FLOAT}, blurScale);
}

void Bloom::genBlurBuffer(vec2 size, float blurScale)
{
    genBlurBuffer(size.x, size.y, blurScale);
}

void Bloom::blurBloom(int intensity, float radius)
{
    gaussianBlur->blur(bloomTexture, intensity, radius);
}

GLuint Bloom::getTexture() const
{
    return gaussianBlur->getTexture();
}
        
void Bloom::render(Shader* shader)
{
    glActiveTexture(GL_TEXTURE0 + 1);
    shader->setInt("blurBloom", 1);
    glBindTexture(GL_TEXTURE_2D, getTexture());
}

Bloom::~Bloom()
{
    delete gaussianBlur;
}
