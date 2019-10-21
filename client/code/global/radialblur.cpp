#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"

#include "radialblur.hpp"

RadialBlur::RadialBlur()
{
    blurShader = new Shader();
    blurShader->loadShaders(global.path("code/shader/radialBlurShader.vert"), global.path("code/shader/radialBlurShader.frag"));
    
    scaleShader = new Shader();
    scaleShader->loadShaders(global.path("code/shader/renderShader.vert"), global.path("code/shader/renderShader.frag"));

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
    blurShader->loadShaders(global.path("code/shader/radialBlurShader.vert"), global.path("code/shader/radialBlurShader.frag"));
    
    scaleShader = new Shader();
    scaleShader->loadShaders(global.path("code/shader/renderShader.vert"), global.path("code/shader/renderShader.frag"));

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

void RadialBlur::genBuffer(int width, int height, FrameBufferData data, float scaleFactor)
{
    colorBuffer->genBuffer(width / scaleFactor, height / scaleFactor, {data});

    downscaleBuffer->genBuffer(width / scaleFactor, height / scaleFactor, {data});
    upscaleBuffer->genBuffer(width, height, {data});
    quad->init();
}

void RadialBlur::genBuffer(vec2 size, FrameBufferData data, float scaleFactor)
{
    genBuffer(size.x, size.y, data, scaleFactor);
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
    colorBuffer->use();

    blurShader->use();

    blurShader->setVec2("center", center);
    blurShader->setFloat("exposure", exposure);
    blurShader->setFloat("decay", decay);
    blurShader->setFloat("density", density);
    blurShader->setFloat("weight", weight);

    glActiveTexture(GL_TEXTURE0);
    blurShader->setInt("blurTexture", 0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    quad->render(blurShader);

    bluredTexture = colorBuffer->getTexture();

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
