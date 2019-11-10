#include "../shader/shader.hpp"
#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../game_object/sphere.hpp"

#include "atmosphere.hpp"

Atmosphere::Atmosphere()
{
    quad = new RenderQuad();
    sphere = new Sphere();
    colorBuffer = new ColorBuffer();

    this->rayOrigin = vec3(0.0);
    this->sunPos = vec3(0.0);
    this->sunIntensity = 0.0;
    this->planetRadius = 0.0;
    this->atmoRadius = 0.0;
    this->rayleighCoeff = vec3(0.0);
    this->mieCoeff = 0.0;
    this->rayleighHeight = 0.0;
    this->mieHeight = 0.0;
    this->mieDir = 0.0;

    this->iBeauty = this->jBeauty = 0;

    this->axis = vec3(1.0, (global.getRandomNumber() - 0.5) * 4, 0.0);
}
        
Atmosphere::Atmosphere(vec3 rayOrigin, vec3 sunPos, float sunIntensity, float planetRadius, float atmoRadius, vec3 rayleighCoeff, float mieCoeff, float rayleighHeight, float mieHeight, float mieDir)
{
    quad = new RenderQuad();
    sphere = new Sphere();
    colorBuffer = new ColorBuffer();
    
    this->rayOrigin = rayOrigin;
    this->sunPos = sunPos;
    this->sunIntensity = sunIntensity;
    this->planetRadius = planetRadius;
    this->atmoRadius = atmoRadius;
    this->rayleighCoeff = rayleighCoeff;
    this->mieCoeff = mieCoeff;
    this->rayleighHeight = rayleighHeight;
    this->mieHeight = mieHeight;
    this->mieDir = mieDir;
    
    this->iBeauty = this->jBeauty = 0;
    
    this->axis = vec3(1.0, (global.getRandomNumber() - 0.5) * 4, 0.0);
}
        
void Atmosphere::genBuffer(int width, int height)
{
    colorBuffer->genBuffer(width, height, {{GL_RGBA16F, GL_RGBA, GL_FLOAT}});

    quad->init();
}

void Atmosphere::genBuffer(vec2 size)
{
    genBuffer(size.x, size.y);
}
        
void Atmosphere::genDome(vec3 center, float radius, int quality)
{
    sphere->construct(center, radius, quality);
}
        
void Atmosphere::setIBeauty(int iBeauty)
{
    this->iBeauty = iBeauty;
}

void Atmosphere::setJBeauty(int jBeauty)
{
    this->jBeauty = jBeauty;
}

void Atmosphere::setRayOrigin(vec3 rayOrigin)
{
    this->rayOrigin = rayOrigin; 
}

void Atmosphere::setSunPos(vec3 sunPos)
{
    this->sunPos = sunPos;
    
    this->axis = vec3(1.0, (global.getRandomNumber() - 0.5) * 4, 0.0);
}

void Atmosphere::setSunIntensity(float sunIntensity)
{
    this->sunIntensity = sunIntensity;
}

void Atmosphere::setPlanetRadius(float planetRadius)
{
    this->planetRadius = planetRadius;
}

void Atmosphere::setAtmoRadius(float atmoRadius)
{
    this->atmoRadius = atmoRadius;
}

void Atmosphere::setRayleighCoeff(vec3 rayleighCoeff)
{
    this->rayleighCoeff = rayleighCoeff;
}

void Atmosphere::setMieCoeff(float mieCoeff)
{
    this->mieCoeff = mieCoeff;
}

void Atmosphere::setRayleighHeight(float rayleighHeight)
{
    this->rayleighHeight = rayleighHeight;
}

void Atmosphere::setMieHeight(float mieHeight)
{
    this->mieHeight = mieHeight;
}

void Atmosphere::setMieDir(float mieDir)
{
    this->mieDir = mieDir;
}

void Atmosphere::renderAtmosphere(Shader* shader)
{
    shader->setInt("iSteps", iBeauty);
    shader->setInt("jSteps", jBeauty);

    shader->setVec3("rayOrigin", rayOrigin);
    shader->setVec3("sunPos", sunPos);
    shader->setFloat("sunIntensity", sunIntensity);
    shader->setFloat("planetRadius", planetRadius);
    shader->setFloat("atmoRadius", atmoRadius);
    shader->setVec3("rayleighCoeff", rayleighCoeff);
    shader->setFloat("mieCoeff", mieCoeff);
    shader->setFloat("rayleighHeight", rayleighHeight);
    shader->setFloat("mieHeight", mieHeight);
    shader->setFloat("mieDir", mieDir);

    glDepthFunc(GL_LEQUAL);

    sphere->render(shader);
    
    glDepthFunc(GL_LESS);
}

void Atmosphere::renderDome(Shader* shader)
{
    if (colorBuffer->getBuffer())
    {
        glDepthFunc(GL_LEQUAL);

        glActiveTexture(GL_TEXTURE0 + 1); 
        shader->setInt("dome", 1);
        glBindTexture(GL_TEXTURE_2D, colorBuffer->getTexture());

        quad->render(shader);

        glDepthFunc(GL_LESS);
    }
}
        
void Atmosphere::updateSunPos()
{
    float theta = 0.03;

    mat4 rotMat = rotate(radians(theta), axis);

    sunPos = vec3(rotMat * vec4(sunPos, 1.0));
}
        
vec3 Atmosphere::getSunPos() const
{
    return sunPos;
}

ColorBuffer* Atmosphere::getBuffer() const
{
    return colorBuffer;
}

GLuint Atmosphere::getTexture(int index) const
{
    return colorBuffer->getTexture(index); 
}

Atmosphere::~Atmosphere()
{
    delete quad;
    delete sphere;
    delete colorBuffer;
}
