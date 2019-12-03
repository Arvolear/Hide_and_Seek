#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"

#include "../window/renderquad.hpp"

#include "../global/radialblur.hpp"
#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

#include "../game_object/sphere.hpp"

#include "dirlightsoftshadow.hpp"
#include "dirlight.hpp"

DirLight::DirLight()
{
    direction = color = vec3(0.0);
    coeff = 1.0;
    shadowViewOffset = 0.0;

    shadowView = shadowProjection = mat4(1.0);

    sphere = new Sphere();

    shadow = new DirLightSoftShadow();
    
    scatterBuffer = new ColorBuffer();
    radialBlur = new RadialBlur();
}

DirLight::DirLight(vec3 direction, vec3 color)
{
    this->direction = direction;
    this->color = color;
    coeff = 1.0;
    shadowViewOffset = 0.0;

    shadowView = shadowProjection = mat4(1.0);

    sphere = new Sphere();

    shadow = new DirLightSoftShadow();
    
    scatterBuffer = new ColorBuffer();
    radialBlur = new RadialBlur();
}

void DirLight::genShadowBuffer(int width, int height)
{
    shadow->genBuffer(width, height);
}

void DirLight::genShadowBuffer(vec2 size)
{
    genShadowBuffer(size.x, size.y);
}

void DirLight::genScatterBuffer(int width, int height, float blurScale)
{
    scatterBuffer->genBuffer(width, height, {{GL_RGBA16F, GL_RGBA, GL_FLOAT}});
    radialBlur->genBuffer(width, height, {GL_RGBA16F, GL_RGBA, GL_FLOAT}, blurScale);
}

void DirLight::genScatterBuffer(vec2 size, float blurScale)
{
    genScatterBuffer(size.x, size.y, blurScale);
}
        
void DirLight::genSphere(vec3 center, double radius, int depth)
{
    sphere->construct(center, radius, depth);
}

void DirLight::setDirection(vec3 dir)
{
    direction = dir;
}

void DirLight::setColor(vec3 color)
{
    this->color = color;
}

void DirLight::setColorCoeff(float coeff)
{
    this->coeff = coeff;
}

void DirLight::setSphereColor(vec3 color)
{
    sphere->setColor(color);
}

void DirLight::setRadialExposure(float exposure)
{
    radialBlur->setExposure(exposure);
}

void DirLight::setRadialDecay(float decay)
{
    radialBlur->setDecay(decay);
}

void DirLight::setRadialDensity(float density)
{
    radialBlur->setDensity(density);
}

void DirLight::setRadialWeight(float weight)
{
    radialBlur->setWeight(weight);
} 

void DirLight::setShadowIntensity(float intensity)
{
    shadow->setIntensity(intensity);
}

void DirLight::setShadowBias(float bias)
{
    shadow->setBias(bias);
}

void DirLight::setShadowSoftness(float softness)
{
    shadow->setSoftness(softness);
}
        
void DirLight::setShadowViewOffset(float offset)
{
    this->shadowViewOffset = offset;
}

vec3 DirLight::getDirection() const
{
    return direction;
}

void DirLight::setShadowProjection(mat4 projection)
{
    this->shadowProjection = projection;
}

void DirLight::blurShadow(int intensity, float radius)
{
    shadow->blurShadow(intensity, radius);
}

void DirLight::blurScatter(vec2 center)
{
    if (scatterBuffer->getBuffer())
    {
        center.x = (center.x + 1.0) / 2.0;
        center.y = (center.y + 1.0) / 2.0;

        radialBlur->blur(scatterBuffer->getTexture(), center);
    }
}

void DirLight::renderShadow(Shader* shader, GLuint index)
{
    shader->setVec3("dirLights[" + to_string(index) + "].direction", direction);
    shader->setVec3("dirLights[" + to_string(index) + "].color", color);
    shader->setFloat("dirLights[" + to_string(index) + "].coeff", coeff);

    GLuint shadowTexture = getShadowTexture();

    if (shadowTexture)
    {
        shader->setInt("dirLights[" + to_string(index) + "].isShadow", 1);

        shadow->render(shader, index);

        shader->setMat4("dirLights[" + to_string(index) + "].shadowView", getShadowView());
        shader->setMat4("dirLights[" + to_string(index) + "].shadowProjection", getShadowProjection());
    }
    else
    {
        shader->setInt("dirLights[" + to_string(index) + "].isShadow", 0);
    }
}

void DirLight::renderSphere(Shader* shader)
{
    glDepthFunc(GL_LEQUAL);

    sphere->render(shader); 

    glDepthFunc(GL_LESS);
}

void DirLight::renderLight(Shader* shader)
{
    glActiveTexture(GL_TEXTURE0);
    shader->setInt("scatterTexture", 0);
    glBindTexture(GL_TEXTURE_2D, getScatterTexture());
}

GLuint DirLight::getShadowTexture() const
{
    return shadow->getTexture();
}

GLuint DirLight::getScatterTexture() const
{
    if (radialBlur->getBuffer() && radialBlur->getTexture())
    {
        return radialBlur->getTexture();
    }
    else if (scatterBuffer->getBuffer())
    {
        return scatterBuffer->getTexture();
    }

    return 0;
}

ShadowBuffer* DirLight::getShadowBuffer() const
{
    return shadow->getBuffer();
}

ColorBuffer* DirLight::getScatterBuffer() const
{
    return scatterBuffer;
}

void DirLight::updateShadowView(vec3 playerPosition, vec3 playerForward)
{
    playerForward = normalize(playerForward);
    vec3 offset = playerForward * shadowViewOffset;

    shadowView = lookAt(offset + playerPosition, offset + playerPosition + normalize(direction), vec3(0, 1, 0));
}

Sphere* DirLight::getSphere() const
{
    return sphere;
}

mat4 DirLight::getShadowView() const
{
    return shadowView;
}

mat4 DirLight::getShadowProjection() const
{
    return shadowProjection;
}

DirLight::~DirLight()
{
    delete shadow;

    delete scatterBuffer;
    delete radialBlur;
}
