#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"

#include "radialblur.hpp"

RadialBlur::RadialBlur()
{
    blurShader = new Shader();
    blurShader->loadShaders(path("code/shader/radialBlurShader.vert"), path("code/shader/radialBlurShader.frag"));
    
    scaleShader = new Shader();
    scaleShader->loadShaders(path("code/shader/renderShader.vert"), path("code/shader/renderShader.frag"));

    colorBuffer = new ColorBuffer();
        
    upscaleBuffer = new ColorBuffer();
    downscaleBuffer = new ColorBuffer();
    
    quad = new RenderQuad();

    bluredTexture = 0;

    exposure = decay = density = weight = 0.0;
}

RadialBlur::RadialBlur(float exposure, float decay, float density, float weight)
{
    blurShader = new Shader();
    blurShader->loadShaders(path("code/shader/radialBlurShader.vert"), path("code/shader/radialBlurShader.frag"));
    
    scaleShader = new Shader();
    scaleShader->loadShaders(path("code/shader/renderShader.vert"), path("code/shader/renderShader.frag"));

    colorBuffer = new ColorBuffer();
        
    upscaleBuffer = new ColorBuffer();
    downscaleBuffer = new ColorBuffer();
    
    quad = new RenderQuad();

    bluredTexture = 0;

    this->exposure = exposure;
    this->decay = decay;
    this->density = density;
    this->weight = weight;
}

void RadialBlur::genBuffer(float width, float height, float scaleFactor)
{
    colorBuffer->genBuffer(width / scaleFactor, height / scaleFactor);

    downscaleBuffer->genBuffer(width / scaleFactor, height / scaleFactor);
    upscaleBuffer->genBuffer(width, height);
    quad->init();
}

void RadialBlur::genBuffer(vec2 size, float scaleFactor)
{
    genBuffer(size.x, size.y, scaleFactor);
}

void RadialBlur::setExposure(float exposure)
{
    this->exposure = exposure;
}

void RadialBlur::setDecay(float decay)
{
    this->decay = decay;
}

void RadialBlur::setDensity(float density)
{
    this->density = density;
}

void RadialBlur::setWeight(float weight)
{
    this->weight = weight;
}
    
GLuint RadialBlur::blur(GLuint textureID, vec2 center)
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
    colorBuffer->use();
    colorBuffer->clear();

    blurShader->use();

    blurShader->setVec2("center", center);
    blurShader->setFloat("exposure", exposure);
    blurShader->setFloat("decay", decay);
    blurShader->setFloat("density", density);
    blurShader->setFloat("weight", weight);

    blurShader->setInt("blurTexture", textureID);
    glActiveTexture(GL_TEXTURE0 + textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    quad->render(blurShader);

    bluredTexture = colorBuffer->getTexture();

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
        
GLuint RadialBlur::getTexture() const
{
    return bluredTexture;
}

GLuint RadialBlur::getBuffer() const
{
    return colorBuffer->getBuffer();
}

RadialBlur::~RadialBlur()
{
    delete blurShader;
    delete scaleShader;

    delete colorBuffer;

    delete upscaleBuffer;
    delete downscaleBuffer;

    delete quad;
}
