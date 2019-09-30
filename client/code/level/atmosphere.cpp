#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"

#include "atmosphere.hpp"

Atmosphere::Atmosphere()
{
    quad = new RenderQuad();
    colorBuffer = new ColorBuffer();

    this->rayOrigin = vec3(0.0, 6372000, 0.0);
    this->sunPos = vec3(0.0, 0.1, -1);
    this->sunIntensity = 22.0;
    this->planetRadius = 6371000;
    this->atmoRadius = 6471000;
    this->rayleighCoeff = vec3(0.0000055, 0.000013, 0.0000224);
    this->mieCoeff = 0.000021;
    this->rayleighHeight = 8000;
    this->mieHeight = 1200;
    this->mieDir = 0.758;
}
        
Atmosphere::Atmosphere(vec3 rayOrigin, vec3 sunPos, float sunIntensity, float planetRadius, float atmoRadius, vec3 rayleighCoeff, float mieCoeff, float rayleighHeight, float mieHeight, float mieDir)
{
    quad = new RenderQuad();
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
}
        
void Atmosphere::genBuffer(int width, int height)
{
    colorBuffer->genBuffer(width, height, 1);
    quad->init();
}

void Atmosphere::genBuffer(vec2 size)
{
    genBuffer(size.x, size.y);
}

void Atmosphere::setRayOrigin(vec3 rayOrigin)
{
    this->rayOrigin = rayOrigin; 
}

void Atmosphere::setSunPos(vec3 sunPos)
{
    this->sunPos = sunPos;
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

void Atmosphere::render(Shader* shader)
{
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

    quad->render(shader);
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
    delete colorBuffer;
}
