#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "lensflare.hpp"

LensFlare::LensFlare()
{
    baseTexture = outTexture = lensDirtTexture = flareBurstTexture = flareGradientTexture = 0;

    flareShader = new Shader();
    flareShader->loadShaders(global.path("code/shader/flareShader.vert"), global.path("code/shader/flareShader.frag"));

    scaleShader = new Shader();
    scaleShader->loadShaders(global.path("code/shader/renderShader.vert"), global.path("code/shader/renderShader.frag"));
    
    gaussianBlur = new GaussianBlur < ColorBuffer >();
    upscaleBuffer = new ColorBuffer();
    downscaleBuffer = new ColorBuffer();
    flareBuffer = new ColorBuffer();
    
    quad = new RenderQuad();
}

void LensFlare::setBaseTexture(GLuint baseTexture)
{
    this->baseTexture = baseTexture;
}
        
void LensFlare::loadHelperTextures(string dirPath)
{
    int W, H; 
    string filename = dirPath + "/flareGradient.png";

    unsigned char* image = SOIL_load_image(filename.data(), &W, &H, 0, SOIL_LOAD_RGBA); 

    glGenTextures(1, &flareGradientTexture); 
    glBindTexture(GL_TEXTURE_2D, flareGradientTexture); 

    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); 
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(image); 
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        throw runtime_error("ERROR::Failed to load texture at path: " + filename);
    }
    
    filename = dirPath + "/lensDirt.png";

    image = SOIL_load_image(filename.data(), &W, &H, 0, SOIL_LOAD_RGBA); 

    glGenTextures(1, &lensDirtTexture); 
    glBindTexture(GL_TEXTURE_2D, lensDirtTexture); 

    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); 
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(image); 
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        throw runtime_error("ERROR::Failed to load texture at path: " + filename);
    }
}

void LensFlare::genBuffer(int width, int height, float scaleFactor)
{
    gaussianBlur->genBuffer(width, height, {GL_RGBA16F, GL_RGBA, GL_FLOAT}, 2);

    downscaleBuffer->genBuffer(width / scaleFactor, height / scaleFactor, {{GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}});
    upscaleBuffer->genBuffer(width, height, {{GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}});
    
    flareBuffer->genBuffer(width / scaleFactor, height / scaleFactor, {{GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}});

    quad->init();
}

void LensFlare::genBuffer(vec2 size, float scale)
{
    genBuffer(size.x, size.y, scale);
}

GLuint LensFlare::getTexture() const
{
    return outTexture;
}

void LensFlare::renderFlares()
{
    glDisable(GL_DEPTH_TEST);

    /* downscale */
    downscaleBuffer->use();

    scaleShader->use();

    glActiveTexture(GL_TEXTURE0);
    scaleShader->setInt("finalTexture", 0);
    glBindTexture(GL_TEXTURE_2D, baseTexture);

    quad->render(scaleShader);

    /* lens flare */
    flareBuffer->use();
    flareBuffer->clear();

    flareShader->use();

    glActiveTexture(GL_TEXTURE0);
    flareShader->setInt("baseTexture", 0);
    glBindTexture(GL_TEXTURE_2D, downscaleBuffer->getTexture());
    
    glActiveTexture(GL_TEXTURE0 + 1);
    flareShader->setInt("flareGradientTexture", 1);
    glBindTexture(GL_TEXTURE_2D, flareGradientTexture);
    
    glActiveTexture(GL_TEXTURE0 + 2);
    flareShader->setInt("lensDirtTexture", 2);
    glBindTexture(GL_TEXTURE_2D, lensDirtTexture);

    quad->render(flareShader);

    /* blur */
    gaussianBlur->blur(flareBuffer->getTexture(), 2);
    
    glDisable(GL_DEPTH_TEST);

    /* upscale */
    upscaleBuffer->use();
    
    scaleShader->use();

    glActiveTexture(GL_TEXTURE0);
    scaleShader->setInt("finalTexture", 0);
    glBindTexture(GL_TEXTURE_2D, gaussianBlur->getTexture());

    quad->render(scaleShader);
    
    outTexture = upscaleBuffer->getTexture();
    
    glEnable(GL_DEPTH_TEST);
}
        
void LensFlare::render(Shader* shader)
{
    glActiveTexture(GL_TEXTURE0 + 2);
    shader->setInt("lensFlare", 2);
    glBindTexture(GL_TEXTURE_2D, getTexture());
}

LensFlare::~LensFlare()
{
    delete flareShader;
    delete scaleShader;

    delete gaussianBlur;
    delete upscaleBuffer;
    delete downscaleBuffer;
    delete flareBuffer;

    delete quad;
}
